
#require assets/opencl/math.cl

typedef struct {
	vec3 orig;
	vec3 invdir;
	int sign[3];
} Ray;

typedef struct {
	int xo;
	int yo;
	int zo;
	int csize;
	int globalid;
	int layerindex;
	int pow8;
	byte oc;
	byte mask;
} Data;

bool intersect(const Ray* r, const vec3 bounds[2], float* dist) {

	real txmin, txmax, tymin, tymax, tzmin, tzmax;

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

inline byte test_octree(vec3 bounds[2], int csize, global byte* octree, int layerid, Ray* ray, int x, int y, int z, int id, int globalid, float* dist, byte* mask) {
	byte vid = 255;
	float tmpdist;
	layerid += globalid + id;

	bounds[0].x = x;
	bounds[0].y = y;
	bounds[0].z = z;
	bounds[1].x = csize + x;
	bounds[1].y = csize + y;
	bounds[1].z = csize + z;

	if ((octree[(layerid) * 4 + 3]) > 128 && ((*mask >> id) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[1].x = csize * 2 + x;

	if ((octree[(layerid + 1) * 4 + 3]) > 128 && ((*mask >> (id + 1)) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[0].z = csize + z;
	bounds[1].z = csize * 2 + z;

	if ((octree[(layerid + 2) * 4 + 3]) > 128 && ((*mask >> (id + 2)) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}

	bounds[0].x = x;
	bounds[1].x = csize + x;

	if ((octree[(layerid + 3) * 4 + 3]) > 128 && ((*mask >> (id + 3)) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
		}

	return vid;
}

void setRotation(vec3* vec, vec3* rotation) {
	float rotated;
	float cosCamX = cos(rotation->x);
	float cosCamY = cos(rotation->y);
	//float cosCamZ = cos(angle.z);
	float sinCamX = sin(rotation->x);
	float sinCamY = sin(rotation->y);
	//float sinCamZ = sin(angle.z);

	/*(p.x * cosCamZ + p.y * sinCamZ, p.y * cosCamZ - p.x * sinCamZ);
	vec->x = rotated.x;
	vec->y = rotated.y;*/

	rotated = vec->z * cosCamX + vec->y * sinCamX;
	vec->y = vec->y * cosCamX - vec->z * sinCamX;
	vec->z = rotated;

	rotated = vec->x * cosCamY - vec->z * sinCamY;
	vec->z = vec->z * cosCamY + vec->x * sinCamY;
	vec->x = rotated;
}

void kernel render(const int spp, const int width, const int height, write_only image2d_t imgb, global float* scnf, global byte* voxsoct, const int octree_depth) {

	// Epic ChadRayFrameworkX
	//   pixel x : get_global_id(0)
	//   pixel y : get_global_id(1)
	//   samples : spp
	//   width   : width
	//   height  : height
	//   texture : imgb
	//   scene   : scnf
	//   voxels  : voxsoct
	//   depth   : octree_depth

	//chunk size, cannot be smaller than 2^octree_depth
	//TODO: automate
	int _csize = 128;
	//pixel id
	int off = get_global_id(0) * 3 + get_global_id(1) * width * 3;

	//calculating ray direction based on pixel coordinates
	const float scale = 0.8f;
	const float imageAspectRatio = (float)width / (float)height;
	vec3 dir;
	dir.x = (2.0f * get_global_id(0) / (float)width - 1.0f) * imageAspectRatio * scale;
	dir.y = (2.0f * get_global_id(1) / (float)height - 1.0f) * scale;
	dir.z = 1;

	//loading scene data
	scene _scene;
	load_scene(&_scene, scnf);

	//background color
	vec3 color = _scene.background;

	//set the rotation of the camera rays 
	vec3 rotation = { _scene.camera_direction.y, _scene.camera_direction.x, 0.0f };
	setRotation(&dir, &rotation);

	//preparing ray
	Ray ray;
	ray.orig = _scene.camera_origin;
	ray.invdir.x = 1 / dir.x;
	ray.invdir.y = 1 / dir.y;
	ray.invdir.z = 1 / dir.z;
	ray.sign[0] = (ray.invdir.x < 0);
	ray.sign[1] = (ray.invdir.y < 0);
	ray.sign[2] = (ray.invdir.z < 0);

	//some variables used later
	vec3 bounds[2] = { { 0, 0, 0 }, { 1, 1, 1 } };
	//distance to the nearest voxel (computed while octree traversal)
	float dist = 0xffffff;
	//id of hit voxel (from 0 to 7, 255 = miss)
	byte oc = 255;
	//coordinates of the currently tested octant
	int xo = 0, yo = 0, zo = 0;
	//size of the currently tested octant
	int csize = _csize;
	//id of tested voxel relative to the start of the currently tested level
	int globalid = 0;
	//id of the first element in the currently tested level
	int layerindex = 1;
	//the power of 8 calculated progressively
	int pow8 = 1;

	//store alternate nodes in case of ray miss
	Data alt_data[10];
	for (int d = 0; d <= octree_depth; d++) {
		alt_data[d].mask = 0b11111111;
	}

	//currently tested level
	int depth = 1;
	for (; depth <= octree_depth; depth++) {
		//get a data container that corresponds to the level of tested node 
		Data* ad = &(alt_data[depth]);

		//store variables in case of having to choose a different path 
		ad->globalid = globalid;
		ad->csize = csize;
		ad->layerindex = layerindex;

		//clearing the closest distance to the voxel
		dist = 0xffffff;
		//decreasing octant size
		csize /= 2;
		//entry to the area where children will be tested
		globalid = globalid * 8;

		//test first 4 octants
		oc = test_octree(bounds, csize, voxsoct, layerindex, &ray, xo, yo, zo, 0, globalid, &dist, &(ad->mask));
		//test next 4 octants
		byte oc1 = test_octree(bounds, csize, voxsoct, layerindex, &ray, xo, yo + csize, zo, 4, globalid, &dist, &(ad->mask));

		//store variables in case of having to choose a different path 
		ad->pow8 = pow8;
		ad->xo = xo;
		ad->yo = yo;
		ad->zo = zo;

		//checking if ray from the second test hit anything
		if (oc1 != 255) oc = oc1;
		//move to the next child (by the id of the one that got intersected)
		globalid += oc;

		//if intersected anything
		if (oc != 255) {
			//move coordinates of the currently tested octant
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

			ad->mask &= ~(1 << oc);
			ad->oc = oc;
			pow8 *= 8;
			layerindex += pow8;

		}
		else {
			if (alt_data[1].mask == 0 || depth == 1)
				break;

			ad->mask = 0b11111111;
			ad = &(alt_data[depth - 1]);
			pow8 = ad->pow8;
			layerindex = ad->layerindex;
			globalid = ad->globalid;
			csize = ad->csize;
			xo = ad->xo;
			yo = ad->yo;
			zo = ad->zo;
			depth -= 2;
		}
	}

	const int index = ((1 - pow8) / -7 + globalid) * 4;
	if (depth >= octree_depth + 1 && voxsoct[index + 3] > 128) {
		color.x = voxsoct[index];
		color.y = voxsoct[index + 1];
		color.z = voxsoct[index + 2];
	}

	int2 coor = { get_global_id(0), get_global_id(1) };
	float4 colr = { color.x / 255.0, color.y / 255.0, color.z / 255.0, 0 };

	write_imagef( imgb, coor, colr );

//	imgb[off + 0] = (byte)color.x;
//	imgb[off + 1] = (byte)color.y;
//	imgb[off + 2] = (byte)color.z;
}
