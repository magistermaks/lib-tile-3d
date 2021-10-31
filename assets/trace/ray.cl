
#require math.cl
#require scene.cl

typedef struct {
	Vec3 orig;
	Vec3 invdir;
	int sign[3];
} Ray;

// check is ray intersects with AABB, writes distance to `dist`
bool intersects(const Ray* r, const Vec3 bounds[2], float* dist) {

	float txmin, txmax, tymin, tymax, tzmin, tzmax;

	txmin = (bounds[r->sign[0]].x - r->orig.x) * r->invdir.x;
	txmax = (bounds[1 - r->sign[0]].x - r->orig.x) * r->invdir.x;
	tymin = (bounds[r->sign[1]].y - r->orig.y) * r->invdir.y;
	tymax = (bounds[1 - r->sign[1]].y - r->orig.y) * r->invdir.y;

	if ((txmin > tymax) || (tymin > txmax)) return false;

	txmin = max(txmin, tymin);
	txmax = min(txmax, tymax);

	tzmin = (bounds[r->sign[2]].z - r->orig.z) * r->invdir.z;
	tzmax = (bounds[1 - r->sign[2]].z - r->orig.z) * r->invdir.z;

	*dist = max(txmin, tzmin);

	return (txmax >= 0) && (tzmax >= 0) && (txmin <= tzmax) && (tzmin <= txmax);

}

void load_direction(Vec3* vec, Vec3* rotation) {
	float rotated;

	const float cosx = cos(rotation->x);
	const float cosy = cos(rotation->y);

	const float sinx = sin(rotation->x);
	const float siny = sin(rotation->y);

	rotated = vec->z * cosx + vec->y * sinx;
	vec->y = vec->y * cosx - vec->z * sinx;
	vec->z = rotated;

	rotated = vec->x * cosy - vec->z * siny;
	vec->z = vec->z * cosy + vec->x * siny;
	vec->x = rotated;
}

void load_ray(Ray* ray, Vec3 direction, Vec3 origin) {

	ray->orig = origin;
	ray->invdir.x = 1.0f / direction.x;
	ray->invdir.y = 1.0f / direction.y;
	ray->invdir.z = 1.0f / direction.z;
	ray->sign[0] = (ray->invdir.x < 0);
	ray->sign[1] = (ray->invdir.y < 0);
	ray->sign[2] = (ray->invdir.z < 0);
}

// initialize ray object
void load_primary_ray(Ray* ray, Scene* scene, int2* pos, const int width, const int height, float fov) {
	const float aspect_ratio = (float) width / (float) height;

	Vec3 dir;
	dir.x = (2.0f * pos->x / (float) width - 1.0f) * aspect_ratio * fov;
	dir.y = (2.0f * pos->y / (float) height - 1.0f) * fov;
	dir.z = 1;
	
	Vec3 rotation = {
		scene->camera_direction.y, 
		scene->camera_direction.x, 
		0.0f 
	};

	load_direction(&dir, &rotation);

	load_ray(ray, dir, scene->camera_origin);
}


