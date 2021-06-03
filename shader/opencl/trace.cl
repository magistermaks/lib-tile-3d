
#require shader/opencl/math.cl

struct Ray {
	vec3 orig;
	vec3 invdir;
	int sign[3];
};

bool intersect(const struct Ray* r, const vec3 bounds[2]) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r->sign[0]].x - r->orig.x) * r->invdir.x;
	tmax = (bounds[1 - r->sign[0]].x - r->orig.x) * r->invdir.x;
	tymin = (bounds[r->sign[1]].y - r->orig.y) * r->invdir.y;
	tymax = (bounds[1 - r->sign[1]].y - r->orig.y) * r->invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[r->sign[2]].z - r->orig.z) * r->invdir.z;
	tzmax = (bounds[1 - r->sign[2]].z - r->orig.z) * r->invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

void kernel render(const int spp, const int width, global byte* imgb, global float* scnf, global byte* voxsoct) {
	int height = 768;
	// Epic ChadRayFrameworkX
	//   pixel x : get_global_id(0)
	//   pixel y : get_global_id(1)
	//   width   : width
	//   texture : imgb
	//   scene   : scnf
	//   samples : spp
	//   voxels  : voxsoct

	int off = get_global_id(0) * 3 + get_global_id(1) * width * 3;

	float sx = 1.0f / (float)width;
	float sy = 1.0f / (float)height;

	vec3 dir;
	dir.x = (float)get_global_id(0) * sx - 0.5f;
	dir.y = (float)get_global_id(1) * sy - 0.5f;
	dir.z = 1.0f;

	vec3 bounds[2] = { { 0, 0, 0 }, { 1, 1, 1 } };

	struct Ray ray;
	scene _scene;
	load_scene(&_scene, scnf);
	ray.orig = _scene.camera_origin;

	ray.invdir.x = 1 / dir.x;
	ray.invdir.y = 1 / dir.y;
	ray.invdir.z = 1 / dir.z;
	ray.sign[0] = (ray.invdir.x < 0);
	ray.sign[1] = (ray.invdir.y < 0);
	ray.sign[2] = (ray.invdir.z < 0);


	imgb[off + 0] = intersect(&ray, bounds) * 255;
	imgb[off + 1] = 0;
	imgb[off + 2] = 0;

}

	
