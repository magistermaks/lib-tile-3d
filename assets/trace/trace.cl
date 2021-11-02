
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
 
	// set background color
	float4 pixel_color = { scene.background.x, scene.background.y, scene.background.z, far };

	const float ray_retreat = 0.01f;

	// preparing ray
	Ray ray;
	load_primary_ray(&ray, &scene, &pos, width, height, tan(fov / 2));
	Ray primary_ray = ray;

	//very random
	const float random[] = { 0.193298, 0.701774, 0.213003, 0.341672, 0.869307, 0.029466, 0.518960, 0.059532, 0.987613, 0.007828, 0.053581, 0.199910, 0.580236, 0.717150, 0.633529, 0.231681, 0.485278, 0.198106, 0.717302, 0.181609 };
	const float jitter = 0.0f;
	int randomid = 0;

	for (int bounces = 0; bounces < 2; bounces++) {
		float4 ray_pixel_color = { scene.background.x, scene.background.y, scene.background.z, far };
		Vec3 hit = { 0,0,0 };
		Vec3 box_pos = { 0,0,0 };
		for (int spp = 0; spp < 2; spp++) {

			float4 color = { scene.background.x, scene.background.y, scene.background.z, far };
			float max_dist = far;
			float size = 64 * scale;

			// iterate all chunks
			for (int chunk = 0; chunk < chunk_count; chunk++) {

				// read chunk offset from chunks array
				Vec3 chunk_pos;
				load_vec3(&chunk_pos, chunks + chunk * 3);
				chunk_pos = muls(&chunk_pos, scale);

				Vec3 bounds[2] = {
					chunk_pos,
					adds(&chunk_pos, size)
				};

				float tmp_dist = 0;
				if (intersects(&ray, bounds, &tmp_dist)) {

					// 222 = csize * sqrt(3)
					if (tmp_dist > -222.0f && tmp_dist < max_dist) {

						// get pointer to octree of given chunk
						global byte* octree = octrees + (chunk * OCTREE_SIZE * VOXEL_SIZE);

						// render the chunk
						Vec3 box_pos_t;
						octree_get_pixel(chunk_pos, &ray, octree, &max_dist, &color, octree_depth, size, &box_pos_t);
						if (spp == 0) box_pos = box_pos_t;
					}
				}
			}

			if (spp == 0) ray_pixel_color = color;
			else {
				ray_pixel_color.x = ((color.w >= far) ? ray_pixel_color.x : ray_pixel_color.x * 0.5f);
				ray_pixel_color.y = ((color.w >= far) ? ray_pixel_color.y : ray_pixel_color.y * 0.5f);
				ray_pixel_color.z = ((color.w >= far) ? ray_pixel_color.z : ray_pixel_color.z * 0.5f);
			}

			if (color.w >= far) {
				break;
			}
			else {
				ray = primary_ray;
				color = ray_pixel_color;

				Vec3 dir = {
					//even more random
					2.0f * (1.0f - random[(pos.x + (int)ray_pixel_color.x + randomid) % 20] * jitter),
					1.0f * (1.0f - random[(pos.y + (int)ray_pixel_color.y + randomid) % 20] * jitter),
					-1.0f * (1.0f - random[(pos.x + (int)ray_pixel_color.z + pos.y + randomid) % 20] * jitter)
				};
				randomid++;
				norm(&dir);

				Vec3 origin = {
					ray.orig.x + (1.0f / ray.invdir.x) * (color.w - ray_retreat),
					ray.orig.y + (1.0f / ray.invdir.y) * (color.w - ray_retreat),
					ray.orig.z + (1.0f / ray.invdir.z) * (color.w - ray_retreat)
				};

				if (spp == 0) hit = origin;

				load_ray(&ray, dir, origin);
			}
		}

		if (ray_pixel_color.w >= far) {
			ray_pixel_color.x = scene.background.x;
			ray_pixel_color.y = scene.background.y;
			ray_pixel_color.z = scene.background.z;
		}

		if (bounces == 0) {
			pixel_color = ray_pixel_color;
		}
		else {
			const float intensity = 0.5f;
			pixel_color.x = ((pixel_color.x + ray_pixel_color.x * intensity) / (1 + intensity), 0.0f, 255.0f);
			pixel_color.y = ((pixel_color.y + ray_pixel_color.y * intensity) / (1 + intensity), 0.0f, 255.0f);
			pixel_color.z = ((pixel_color.z + ray_pixel_color.z * intensity) / (1 + intensity), 0.0f, 255.0f);
		}

		if (ray_pixel_color.w >= far) break;
		Bounce(&primary_ray, hit, box_pos, 1.0f);
		ray = primary_ray;
	}

	float4 colr = {
		pixel_color.x * (1.0f / 255.0f),
		pixel_color.y * (1.0f / 255.0f),
		pixel_color.z * (1.0f / 255.0f),
		(1.0f/ pixel_color.w - 1.0f/near) / (1.0f/far - 1.0f/near),
	};

	write_imagef(image, pos, colr);

}

