
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

byte test_octree(vec3 bounds[2], int csize, global byte* octree, int layerid, Ray* ray, int x, int y, int z, int id, int globalid, float* dist, byte* mask) {
	byte vid = 255;
	float tmpdist;
	layerid += globalid + id;

	bounds[0].x = x;
	bounds[0].y = y;
	bounds[0].z = z;
	bounds[1].x = csize + x;
	bounds[1].y = csize + y;
	bounds[1].z = csize + z;

	if ((octree[(layerid) * 4 + 3]) > 0 && ((*mask >> id) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[1].x = csize * 2 + x;

	if ((octree[(layerid + 1) * 4 + 3]) > 0 && ((*mask >> (id + 1)) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}

	bounds[0].x = csize + x;
	bounds[0].z = csize + z;
	bounds[1].z = csize * 2 + z;

	if ((octree[(layerid + 2) * 4 + 3]) > 0 && ((*mask >> (id + 2)) & 1) == 1)
		if (intersect(ray, bounds, &tmpdist)) {
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}

	bounds[0].x = x;
	bounds[1].x = csize + x;

	if ((octree[(layerid + 3) * 4 + 3]) > 0 && ((*mask >> (id + 3)) & 1) == 1)
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

	float sinCamX = sin(rotation->x);
	float sinCamY = sin(rotation->y);

	rotated = vec->z * cosCamX + vec->y * sinCamX;
	vec->y = vec->y * cosCamX - vec->z * sinCamX;
	vec->z = rotated;

	rotated = vec->x * cosCamY - vec->z * sinCamY;
	vec->z = vec->z * cosCamY + vec->x * sinCamY;
	vec->x = rotated;
}

void render_chunk( float cx, float cy, float cz, Ray* ray, global byte* octree, float* max_dist, vec3* output, int octree_depth, int csize ) {

	// some variables used later
	vec3 bounds[2] = { { 0, 0, 0 }, { 1, 1, 1 } };

	float dist = 0xffffff;

	// id of hit voxel (from 0 to 7, 255 = miss)
	byte oc = 255;

	// coordinates of the currently tested octant
	int xo = 0, yo = 0, zo = 0;

	// id of tested voxel relative to the start of the currently tested level
	int globalid = 0;

	// id of the first element in the currently tested level
	int layerindex = 1;

	// the power of 8 calculated progressively
	int pow8 = 1;

	// store alternate nodes in case of ray miss
	Data alt_data[10];
	for (int d = 0; d <= octree_depth; d++) {
		alt_data[d].mask = 0b11111111;
	}

	// currently tested level
	int depth = 1;
	for (; depth <= octree_depth; depth++) {

		// get a data container that corresponds to the level of tested node 
		Data* ad = &(alt_data[depth]);

		// store variables in case of having to choose a different path 
		ad->globalid = globalid;
		ad->csize = csize;
		ad->layerindex = layerindex;

		// clearing the closest distance to the voxel
		dist = 0xffffff;

		// decreasing octant size
		csize /= 2;

		// entry to the area where children will be tested
		globalid = globalid * 8;

		// test first 4 octants
		oc = test_octree(bounds, csize, octree, layerindex, ray, xo + cx, yo + cy, zo + cz, 0, globalid, &dist, &(ad->mask));

		// test next 4 octants
		byte oc1 = test_octree(bounds, csize, octree, layerindex, ray, xo + cx, yo + csize + cy, zo + cz, 4, globalid, &dist, &(ad->mask));

		// store variables in case of having to choose a different path 
		ad->pow8 = pow8;
		ad->xo = xo;
		ad->yo = yo;
		ad->zo = zo;

		// checking if ray from the second test hit anything
		if (oc1 != 255) oc = oc1;

		// move to the next child (by the id of the one that got intersected)
		globalid += oc;

		// if intersected anything
		if (oc != 255) {

			// move coordinates of the currently tested octant
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

		} else {
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

	if( dist < *max_dist ) {
		*max_dist = dist;

		const int index = ((1 - pow8) / -7 + globalid) * 4;
		if (depth >= octree_depth + 1 && octree[index + 3] > 0) {
			output->x = octree[index];
			output->y = octree[index + 1];
			output->z = octree[index + 2];
		}
	}
}

void kernel render( const int spp, const int width, const int height, const int octree_depth, const int chunk_count, write_only image2d_t image, global float* scnf, global byte* octrees, global float* chunks ) {

	// Epic ChadRayFrameworkX
	//   pixel x       : get_global_id(0)
	//   pixel y       : get_global_id(1)
	//   samples/pixel : spp
	//   width         : image width
	//   height        : image height
	//   output image  : image
	//   scene         : scnf
	//   octree array  : octrees 
	//   depth         : octree_depth (<= 6)
	//   chunk count   : chunk_count
	//   chunk offsets : chunks 

	int2 pos = { 
		get_global_id(0),
		get_global_id(1) 
	};

	// calculating ray direction based on pixel coordinates
	const float scale = 0.8f;
	const float aspect_ratio = (float) width / (float) height;

	// size of the currently tested octant
	// chunk size, cannot be smaller than 2^octree_depth
	// TODO: do something smart with this
	const int csize = 128;

	vec3 dir;
	dir.x = (2.0f * pos.x / (float) width - 1.0f) * aspect_ratio * scale;
	dir.y = (2.0f * pos.y / (float) height - 1.0f) * scale;
	dir.z = 1;

	// load scene data
	Scene scene;
	load_scene(&scene, scnf);

	// background color
	vec3 color = scene.background;

	// set the rotation of the camera rays 
	vec3 rotation = { scene.camera_direction.y, scene.camera_direction.x, 0.0f };
	setRotation(&dir, &rotation);

	// preparing ray
	Ray ray;
	ray.orig = scene.camera_origin;
	ray.invdir.x = 1 / dir.x;
	ray.invdir.y = 1 / dir.y;
	ray.invdir.z = 1 / dir.z;
	ray.sign[0] = (ray.invdir.x < 0);
	ray.sign[1] = (ray.invdir.y < 0);
	ray.sign[2] = (ray.invdir.z < 0);

	float max_dist = 0xffffff;

	// iterate all chunks
	for( int chunk = 0; chunk < chunk_count; chunk ++ ) {
		
		// read chunk offset from chunks array
		float cx = chunks[chunk * 3 + 0] * 2;
		float cy = chunks[chunk * 3 + 1] * 2;
		float cz = chunks[chunk * 3 + 2] * 2;

		vec3 bounds[2] = { { cx, cy, cz }, { cx + csize, cy + csize, cz + csize } };
		float useless_distance = 0;

		if ( intersect(&ray, bounds, &useless_distance )) {

			// get pointer to octree of given chunk
			// the 299593 is derived from: `((1 - pow(8, (octree_depth + 1))) / -7)`
			global byte* octree = octrees + (chunk * 299593 * 4);

			// render the chunk
			// internally updates `max_dist`
			render_chunk( cx, cy, cz, &ray, octree, &max_dist, &color, octree_depth, csize );
		}
	}

	float4 colr = { 
		color.x * (1.0f / 255.0f), 
		color.y * (1.0f / 255.0f), 
		color.z * (1.0f / 255.0f), 
		0 
	};

	write_imagef( image, pos, colr );

}
