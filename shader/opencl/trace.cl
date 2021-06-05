
#require shader/opencl/math.cl

typedef struct {
	vec3 orig;
	vec3 invdir;
	int sign[3];
} Ray;

bool intersect(const Ray* r, const vec3 bounds[2]) {

	real txmin, txmax, tymin, tymax, tzmin, tzmax;

	txmin = ( bounds[    r->sign[0]].x - r->orig.x ) * r->invdir.x;
	txmax = ( bounds[1 - r->sign[0]].x - r->orig.x ) * r->invdir.x;
	tymin = ( bounds[    r->sign[1]].y - r->orig.y ) * r->invdir.y;
	tymax = ( bounds[1 - r->sign[1]].y - r->orig.y ) * r->invdir.y;

	if( (txmin > tymax) || (tymin > txmax) ) return false;

	txmin = max( txmin, tymin );
	txmax = min( txmax, tymax );

	tzmin = ( bounds[    r->sign[2]].z - r->orig.z ) * r->invdir.z;
	tzmax = ( bounds[1 - r->sign[2]].z - r->orig.z ) * r->invdir.z;

	return (txmin <= tzmax) && (tzmin <= txmax);

}

inline float vdist(vec3 origin, vec3 bounds[2], int csize) {
	vec3 pivot = { (float)csize * 0.5f, (float)csize * 0.5f, (float)csize * 0.5f };
	pivot = add(&pivot, &bounds[0]);
	pivot = sub(&pivot, &origin);
	const float d = pivot.x * pivot.x + pivot.y * pivot.y + pivot.z * pivot.z;
	return d;
}

inline byte test_octree(vec3 bounds[2], int csize, global byte* octree, int layerid, Ray* ray, int x, int y, int z, int id, int globalid, float* dist) {
	byte vid = 255;
	float tmpdist;
	layerid += globalid;

	bounds[0].x = x;
	bounds[0].y = y;
	bounds[0].z = z;
	bounds[1].x = csize + x;
	bounds[1].y = csize + y;
	bounds[1].z = csize + z;

	if ((octree[(layerid + id) * 4 + 3]) > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(ray->orig, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[1].x = csize * 2 + x;

	if ((octree[(layerid + 1 + id) * 4 + 3]) > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(ray->orig, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[0].z = csize + z;
	bounds[1].z = csize * 2 + z;

	if ((octree[(layerid + 2 + id) * 4 + 3]) > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(ray->orig, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}

	bounds[0].x = x;
	bounds[1].x = csize + x;

	if ((octree[(layerid + 3 + id) * 4 + 3]) > 128)
		if (intersect(ray, bounds)) {
			tmpdist = vdist(ray->orig, bounds, csize);
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
		}

	return vid;
}


void kernel render(const int spp, const int width, global byte* imgb, global float* scnf, global byte* voxsoct, const int octree_depth) {
	int height = 768;
	// Epic ChadRayFrameworkX
	//   pixel x : get_global_id(0)
	//   pixel y : get_global_id(1)
	//   width   : width
	//   texture : imgb
	//   scene   : scnf
	//   samples : spp
	//   voxels  : voxsoct

	//int octree_depth = 4;
	int _csize = 64;
	int off = get_global_id(0) * 3 + get_global_id(1) * width * 3;

	float sx = 1.0f / (float)width;
	float sy = 1.0f / (float)height;

	vec3 dir;
	dir.x = (float)get_global_id(0) * sx - 0.5f;
	dir.y = (float)get_global_id(1) * sy - 0.5f;
	dir.z = 1.0f;

	scene _scene;
	load_scene(&_scene, scnf);

	vec3 color = _scene.background;
	vec3 bounds[2] = { { 0, 0, 0 }, { 1, 1, 1 } };



	Ray ray;
	ray.orig = _scene.camera_origin;

	ray.invdir.x = 1 / dir.x;
	ray.invdir.y = 1 / dir.y;
	ray.invdir.z = 1 / dir.z;
	ray.sign[0] = (ray.invdir.x < 0);
	ray.sign[1] = (ray.invdir.y < 0);
	ray.sign[2] = (ray.invdir.z < 0);

	float dist = 0xffffff;
	byte oc = 255;
	int xo = 0, yo = 0, zo = 0;
	int csize = _csize;
	int globalid = 0;	
	int layerindex = 1;
	int pow8 = 1;

	int depth = 1;
	for (; depth <= octree_depth; depth++) {
		dist = 0xffffff;
		csize /= 2;

		oc = test_octree(bounds, csize, voxsoct, layerindex - 1, &ray, xo, yo, zo, 0, globalid, &dist);
		byte oc1 = test_octree(bounds, csize, voxsoct, layerindex - 1, &ray, xo, yo + csize, zo, 4, globalid, &dist);

		if (oc1 != 255) oc = oc1;

		if (oc != 255) {
			globalid = globalid * 8 + oc;

			layerindex += pow8;
			pow8 *= 8;

			switch (oc) {
			case 1:
				xo += csize;
				break;

			case 2:
				xo += csize;
				zo += csize;
				break;

			case 3:
				zo += csize;
				break;

			case 4:
				yo += csize;
				break;

			case 5:
				xo += csize;
				yo += csize;
				break;

			case 6:
				xo += csize;
				yo += csize;
				zo += csize;
				break;

			case 7:
				yo += csize;
				zo += csize;
				break;

			default:
				break;
			}



		}
		else break;
	}

	const int index = ((1 - pow8) / -7 + globalid - 1) * 4;//((1 - pow(8, (int)((octree_depth))) / -7 + globalid - 1)) * 4;
	if (depth >= octree_depth + 1 && voxsoct[index + 3] > 128) {
		color.x = voxsoct[index];
		color.y = voxsoct[index + 1];
		color.z = voxsoct[index + 2];
	}

	imgb[off + 0] = (byte)color.x;
	imgb[off + 1] = (byte)color.y;
	imgb[off + 2] = (byte)color.z;

}

