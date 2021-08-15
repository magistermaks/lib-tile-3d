
#require assets/opencl/math.cl

typedef struct {
	vec3 orig;
	vec3 invdir;
	int sign[3];
} Ray;

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

global byte* render_xam_branch( vec3 origin, Ray* ray, global byte* octree, int size, float* dist, float len ) {
	
	global byte* branch = 0;
	float tmp_dist;
	vec3 moved;

	for( int i = 0; i < 8; i ++ ) {

		if( octree[0] != 0 ) {

			vec3 pos = {
				origin.x + !!(i & 1) * len, 
				origin.y + !!(i & 2) * len, 
				origin.z + !!(i & 4) * len
			};

			vec3 bounds[2] = {origin, pos};

			if( intersect(ray, bounds, &tmp_dist) ) {

				// TODO: why >=?
				if( *dist >= tmp_dist ) {
					*dist = tmp_dist;
					branch = octree;
					moved = pos;
				}

			}
		}

		// move to the next child
		octree += size;

	}

	// we go a hit with the tree
	if( branch != 0 ) {
		
		// return voxel pointer
		if( size == 1 ) return branch;

		// go to the lower layer
		return render_xam_branch( moved, ray, branch + 1, (size - 1) >> 3, dist, len * 0.5f );

	}

	// return nullptr
	return 0;

}

void render_xam_chunk( float cx, float cy, float cz, Ray* ray, global byte* octree, float* dist, vec3* output ) {
	
	// the octree is empty
	if( octree[0] == 0 ) {
		return;
	}

	// just for now
	*dist = 0xffffff;

	int octree_length = 299593;
	int size = (octree_length - 1) >> 3;
	vec3 chunk_pos = { cx, cy, cz };

	global byte* voxel = render_xam_branch( chunk_pos, ray, octree + 1, size, &dist, 128 );

	if( voxel != 0 ) {
		output->x = voxel[0];
		output->y = voxel[1];
		output->z = voxel[2];
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

	// this isn't realy applicable anymore, remove in the future
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

	float dist = 0xffffff;
	float dummy = 0;

	// iterate all chunks
	for( int chunk = 0; chunk < chunk_count; chunk ++ ) {
		
		// read chunk offset from chunks array
		float cx = chunks[chunk * 3 + 0] * 2;
		float cy = chunks[chunk * 3 + 1] * 2;
		float cz = chunks[chunk * 3 + 2] * 2;

		vec3 bounds[2] = { { cx, cy, cz }, { cx + csize, cy + csize, cz + csize } };

		if( intersect(&ray, bounds, &dummy) ) {

			// get pointer to octree of given chunk
			// the 299593 is derived from: `((1 - pow(8, octree_depth + 1)) / -7)`
			global byte* octree = octrees + (chunk * 299593 * 4);

			// render the chunk
			// internally updates `dist`
			render_xam_chunk( cx, cy, cz, &ray, octree, &dist, &color );
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
