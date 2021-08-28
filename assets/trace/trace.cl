
#require octree.cl
#require scene.cl
#require ray.cl

#define MAX_DEPTH 1000

float get_depth( float dist ) {
	const float far = MAX_DEPTH;
	const float near = 0.1f;

	return (1.0f/dist - 1.0f/near) / (1.0f/far - 1.0f/near);
}

void kernel main( const int spp, const int width, const int height, const int octree_depth, const int chunk_count, write_only image2d_t image, global float* scnf, global byte* octrees, global float* chunks, const byte render_mode ) {

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

	// size of one voxel
	float scale = 2;

	int2 pos = { 
		get_global_id(0) * ((render_mode >= 16) ? 2 : 1),
		get_global_id(1) * ((render_mode >= 8) ? 2 : 1)
	};

	// selects pixels which are rendered (if "blur" enabled)
	switch (render_mode) {

		case (8 + 1):
			pos.y += 1;
			break;

		case (16 + 1):
			pos.x += 1;
			break;

		case (16 + 2):
			pos.y += 1;
			break;

		case (16 + 3):
			pos.x += 1;
			pos.y += 1;
			break;

		default:
			break;
	}

	// load scene data
	Scene scene;
	load_scene(&scene, scnf);

	// preparing ray
	Ray ray;
	load_ray(&ray, &scene, &pos, width, height, tan(77.5f * 0.01745329 / 2));
 
	// set background color
	float4 color = { scene.background.x, scene.background.y, scene.background.z, MAX_DEPTH };

	float max_dist = 0xffffff;
	float size = 64 * scale;

	// iterate all chunks
	for( int chunk = 0; chunk < chunk_count; chunk ++ ) {

		// read chunk offset from chunks array
		Vec3 chunk_pos;
		load_vec3(&chunk_pos, chunks + chunk * 3);
		chunk_pos = muls(&chunk_pos, scale);

		Vec3 bounds[2] = { 
			chunk_pos, 
			adds(&chunk_pos, size)
		};

		float tmp_dist = 0;
		if( intersects(&ray, bounds, &tmp_dist) ) {

			// 222 = csize * sqrt(3)
			if( tmp_dist > -222.0f && tmp_dist < max_dist ) {

				// get pointer to octree of given chunk
				// the 299593 is derived from: `((1 - pow(8, (octree_depth + 1))) / -7)`
				global byte* octree = octrees + (chunk * 299593 * VOXEL_SIZE);

				// render the chunk
				// internally updates `max_dist`
				octree_get_pixel(chunk_pos, &ray, octree, &max_dist, &color, octree_depth, size);
			}
		}
	}

	float4 colr = {
		color.x * (1.0f / 255.0f),
		color.y * (1.0f / 255.0f),
		color.z * (1.0f / 255.0f),
		get_depth(color.w)
	};

	write_imagef(image, pos, colr);

}

