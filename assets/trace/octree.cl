
#require math.cl
#require ray.cl

// the 299593 is derived from: `((1 - pow(8, (octree_depth + 1))) / -7)`
#define OCTREE_SIZE 299593

// size of single octree leaf
#define VOXEL_SIZE 4

// channel offsets
#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

typedef struct {
	Vec3 xyzo;
	float csize;
	int globalid;
	int layerindex;
	int pow8;
	byte oc;
	byte mask;
} OctreeData;

// get octant id of ray-octant intersection
byte octree_test_octant(const float csize, global byte* octree, const Ray* ray, Vec3 xyz, const int id, float* dist, byte mask) {

	if( id > 0 ) xyz.z += csize;

	byte vid = 255;
	float tmpdist;

	Vec3 bounds[2] = { xyz, { csize + xyz.x, csize + xyz.y, csize + xyz.z } };

	if( (mask >> id) & 1 ) {
		if( intersects(ray, bounds, &tmpdist) ) {
			if (*dist >= tmpdist) {
				vid = id;
				*dist = tmpdist;
			}
		}
	}

	bounds[0].x = csize + xyz.x;
	bounds[1].x = csize * 2 + xyz.x;

	if( (mask >> (id + 1)) & 1 ) {
		if( intersects(ray, bounds, &tmpdist) ) {
			if (*dist >= tmpdist) {
				vid = id + 1;
				*dist = tmpdist;
			}
		}
	}

	bounds[0].y = csize + xyz.y;
	bounds[1].y = csize * 2 + xyz.y;

	if( (mask >> (id + 3)) & 1 ) {
		if( intersects(ray, bounds, &tmpdist) ) {
			if (*dist >= tmpdist) {
				vid = id + 3;
				*dist = tmpdist;
			}
		}
	}

	if( (mask >> (id + 2)) & 1 ) {
		bounds[0].x = xyz.x;
		bounds[1].x = csize + xyz.x;
		if( intersects(ray, bounds, &tmpdist) ) {
			if (*dist >= tmpdist) {
				vid = id + 2;
				*dist = tmpdist;
			}
		}
	}

	return vid;
}

// draw single pixel queried from octree with a 3D ray
void octree_get_pixel(Vec3 xyzc, Ray* ray, global byte* octree, float* max_dist, float4* output, int octree_depth, float csize, Vec3* box_pos) {

	float dist;

	// id of hit voxel (from 0 to 7, 255 = miss)
	byte oc = 255;

	// coordinates of the currently tested octant
	Vec3 xyzo = { 0, 0, 0 };

	// id of tested voxel relative to the start of the currently tested level
	int globalid = 0;

	// id of the first element in the currently tested level
	int layerindex = 1;

	// the power of 8 calculated progressively
	int pow8 = 1;

	// store alternate nodes in case of ray miss
	OctreeData alt_data[7];
	for (int d = 0; d <= octree_depth; d++) {
		alt_data[d].mask = 0b11111111;
	}

	// currently tested level 
	int depth = 1;
	for(; depth <= octree_depth; depth ++ ) {

		// get a data container that corresponds to the level of tested node 
		OctreeData* ad = &alt_data[depth];

		// store variables in case of having to choose a different path 
		ad->globalid = globalid;
		ad->csize = csize;
		ad->layerindex = layerindex;
		ad->pow8 = pow8;
		ad->xyzo = xyzo;

		// mask representing transparency of children
		byte alpha_mask = ad->mask & octree[(layerindex - pow8 + globalid) * VOXEL_SIZE + ALPHA];

		// clearing the closest distance to the voxel
		dist = *max_dist;

		// decreasing octant size
		csize /= 2.0f;

		// entry to the area where children will be tested
		globalid *= 8;

		// test first 4 octants
		if( ad->mask & 0b00001111 )
			oc = octree_test_octant(csize, octree, ray, add(&xyzo, &xyzc), 0, &dist, alpha_mask);

		// test next 4 octants
		if( ad->mask & 0b11110000 ) {
			byte oc1 = octree_test_octant(csize, octree, ray, add(&xyzo, &xyzc), 4, &dist, alpha_mask);

			// checking if ray from the second test hit anything
			if( oc1 != 255 ) oc = oc1;
		}

		// move to the next child (by the id of the one that got intersected)
		globalid += oc;

		// if intersected anything
		if( oc != 255 ) {

			// move coordinates of the currently tested octant
			xyzo.x += !!(oc & 1) * csize;
			xyzo.y += !!(oc & 2) * csize;
			xyzo.z += !!(oc & 4) * csize;

			ad->mask &= ~(1 << oc);
			ad->oc = oc;
			pow8 *= 8;
			layerindex += pow8;

		}else{
			if( alt_data[1].mask == 0 || depth == 1 )
				break;

			ad->mask = 0b11111111;
			ad = &alt_data[depth - 1];
			pow8 = ad->pow8;
			layerindex = ad->layerindex;
			globalid = ad->globalid;
			csize = ad->csize;
			xyzo = ad->xyzo;
			depth -= 2;
		}
	}

	if( dist < *max_dist ) {
		*max_dist = dist;

		const int index = ((1 - pow8) / -7 + globalid) * VOXEL_SIZE;
		output->x = octree[index + RED];
		output->y = octree[index + GREEN];
		output->z = octree[index + BLUE];
		output->w = dist;

		*box_pos = add(&xyzo, &xyzc);
		box_pos->x += 1;
		box_pos->y += 1;
		box_pos->z += 1;
	}
}

