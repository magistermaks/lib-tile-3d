
#include "worldgen.hpp"

void Worldgen::gen_chunk_world(Region& region) {

	for (int i = 0; i <= 2; i += 2) {
		for (int xc = -2; xc <= 2; xc++) {
			for (int zc = -2; zc <= 2; zc++) {
				int x = xc, z = zc, y = i;

				region.put(nullptr, x, y, z);
				VoxelTree& tree = *region.chunk(x, y, z)->tree;

				x *= 64;
				y *= 64;
				z *= 64;

				const siv::PerlinNoise perlin(8888);
				const double frequency = 2.0;
				const double fx = 64.0 / frequency;
				const double fs = 128.0 / frequency;

				for (int _x = 0; _x < 64; _x++) {
					for (int _z = 0; _z < 64; _z++) {

						if (y < 128) {
							byte surface = (byte)(perlin.accumulatedOctaveNoise2D_0_1((double)(x + _x) / fs, (double)(z + _z) / fs, 8) * 16 + 48);

							for (int _y = 0; _y < 64; _y++) {
								if (_y < surface) {
									byte value = (byte)(perlin.accumulatedOctaveNoise3D_0_1((double)(x + _x) / fx, (double)(y + _y) / fx, (double)(z + _z) / fx, 8) * 255);
									byte clp = std::clamp(value - 100, 0, 1) * 255;

									byte r = value, g = value, b = value;

									// grass layer
									if (_y > surface - 4) {
										r = 0; b = 0;
										g = std::clamp((int)value, 100, 200);
									}
									// dirt layer
									else if (_y > surface - 8) {
										g = 60; b = 0;
										r = std::clamp((int)value, 90, 180);
									}
									// random dirt
									else if ((byte)rand() < 8) {
										g = 60; b = 0;
										r = std::clamp((int)value, 90, 180);
									}

									tree.set(_x, _y, _z, {
										r, g, b, clp
										});
								}
							}
						}
						else {
							// clouds
							for (int _y = 32; _y < 64; _y++) {
								byte value = (byte)(perlin.accumulatedOctaveNoise3D_0_1((double)(x + _x) / 64, (double)(y + _y) / 64, (double)(z + _z) / 64, 6) * 255);
								byte clp = std::clamp(value - 200, 0, 1) * 255;

								byte r = value, g = value, b = value;

								tree.set(_x, _y, _z, {
									r, g, b, clp
									});
							}
						}
					}
				}
			}
		}
	}
}

void Worldgen::gen_chunk_scene1(Region& region) {

	region.put(nullptr, 0, 0, 0);

	VoxelTree& tree = *region.chunk(0, 0, 0)->tree;

	tree.set(0, 0, 0, { 255, 0, 0, 255 });
	tree.set(1, 1, 0, { 0, 255, 0, 255 });
	tree.set(2, 2, 0, { 0, 0, 255, 255 });
	tree.set(3, 3, 0, { 255, 255, 0, 255 });
	tree.set(4, 4, 0, { 0, 255, 255, 255 });
	tree.set(5, 5, 0, { 255, 0, 255, 255 });


	for (int xc = -2; xc <= 2; xc++) {
		for (int zc = -2; zc <= 2; zc++) {
			if (zc == 1 && xc == 0) continue;
			if (xc == 0 && zc == 0) continue;
			if (zc == -1 && xc == 0) continue;
			if (zc == 1 && xc == 1) continue;
			if (zc == -1 && xc == 1) continue;
			if (zc == 1 && xc == -1) continue;
			if (zc == -1 && xc == -1) continue;
			if (zc == 0 && xc == -1) continue;
			if (zc == 0 && xc == 1) continue;

			region.put(nullptr, xc, 0, zc);
			VoxelTree& tree = *region.chunk(xc, 0, zc)->tree;

			for (int x = 0; x < 64; x++) {
				for (int y = 0; y < 50; y++) {
					for (int z = 0; z < 64; z++) {
						tree.set(x, y, z, {
							((byte)rand()), ((byte)rand()), ((byte)rand()), 255
							});
					}
				}
			}
		}
	}
}
