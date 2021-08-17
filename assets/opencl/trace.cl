
#require assets/opencl/math.cl
#define VOXEL_SIZE 4

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

// god forgive me
// expects: `tmp_dist`, `voxel_dist`, `origin`, `octree`, `ray`
#define OCTREE_LAYER( size, len, ... ) { \
	global byte* ptr##size = octree; \
	octree += VOXEL_SIZE; \
	for( int i##size = 0; i##size < 8; i##size ++ ) { \
		if( octree[3] ) { \
			vec3 pos##size = { \
				origin.x + !!(i##size & 1) * len, \
				origin.y + !!(i##size & 2) * len, \
				origin.z + !!(i##size & 4) * len \
			}; \
			vec3 bounds##size[2] = { \
				pos##size, \
				adds(&pos##size, len) \
			}; \
			vec3 origin##size = origin; \
			origin = pos##size; \
			if( intersect(ray, bounds##size, &tmp_dist) ) { \
				if( *voxel_dist > tmp_dist ) { \
					{ __VA_ARGS__ } \
				} \
			} \
			origin = origin##size; \
		} \
		octree += size * VOXEL_SIZE; \
	} \
	octree = ptr##size; \
}

global byte* octree_get_voxel( vec3 origin, Ray* ray, global byte* octree, float* voxel_dist ) {

	// the octree is empty
	if( octree[3] == 0 ) {
		return nullptr;
	}

	global byte* voxel = nullptr;
	float tmp_dist;

	OCTREE_LAYER( 37449, 64, {
		OCTREE_LAYER( 4681, 32, {
			OCTREE_LAYER( 585, 16, {
				OCTREE_LAYER( 73, 8, {
					OCTREE_LAYER( 9, 4, {
						OCTREE_LAYER( 1, 2, {

							*voxel_dist = tmp_dist;
							voxel = octree;

						} );
					} );
				} );
			} );
		} );
	} );

	return voxel;

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
		vec3 pos = {
			chunks[chunk * 3 + 0] * 2,
			chunks[chunk * 3 + 1] * 2,
			chunks[chunk * 3 + 2] * 2
		};

		// chunk bounds
		vec3 bounds[2] = { pos, adds(&pos, 128) };

		if( intersect(&ray, bounds, &dummy) ) {

			// get pointer to octree of given chunk
			// the 299593 is derived from: `((1 - pow(8, octree_depth + 1)) / -7)`
			global byte* octree = octrees + (chunk * 299593 * VOXEL_SIZE);

			// render the chunk
			// internally updates `dist`
			global byte* voxel = octree_get_voxel( pos, &ray, octree, &dist );

			// draw voxel to screen (if voxel != nullptr)
			if( voxel ) {
				color.x = voxel[0];
				color.y = voxel[1];
				color.z = voxel[2];
			}
			
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
