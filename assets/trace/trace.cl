
#require octree.cl
#require scene.cl
#require ray.cl

void kernel main(
	const int spp,              // samples-per-pixel (unused)
	const int width,            // width of the screen buffer
	const int height,           // height of the screen buffer
	const int octree_depth,     // depth of voxel octree (<= 6)
	const int chunk_count,      // the number of chunks to render
	write_only image2d_t image, // output RGB (+ depth) screen buffer
	global float* scnf,         // scene data array
	global byte* octrees,       // array of chunks
	global float* chunks,       // array of chunk coordinates
	const byte render_mode      // pixel pos offset mode
) {

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
	}

	// load scene data
	Scene scene;
	load_scene(&scene, scnf);

	const float scale = 2;
	const float fov = scene.projection.x;
	const float near = scene.projection.y;
	const float far = scene.projection.z;

	// preparing ray
	Ray ray;
	load_ray(&ray, &scene, &pos, width, height, tan(fov / 2));
 
	// set background color
	float4 color = { scene.background.x, scene.background.y, scene.background.z, far };

	float max_dist = far;
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
				global byte* octree = octrees + (chunk * OCTREE_SIZE * VOXEL_SIZE);

				// render the chunk
				octree_get_pixel(chunk_pos, &ray, octree, &max_dist, &color, octree_depth, size);
			}
		}
	}

	float4 colr = {
		color.x * (1.0f / 255.0f),
		color.y * (1.0f / 255.0f),
		color.z * (1.0f / 255.0f),
		(1.0f/color.w - 1.0f/near) / (1.0f/far - 1.0f/near),
	};

	write_imagef(image, pos, colr);

}

