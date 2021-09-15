
#include "worldgen.hpp"

void Worldgen::scene_world(World& world) {

	world.setGenerator( [] (World* world, Chunk* chunk, int x, int y, int z) {
		auto& tree = *chunk->tree;

		const siv::PerlinNoise perlin(8888);
		const double frequency = 2.0;
		const double fx = 64.0 / frequency;
		const double fs = 128.0 / frequency;

		x *= 64;
		y *= 64;
		z *= 64;

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
				} else { // clouds
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
	} );

	for (int i = 0; i <= 2; i += 2) {
		for (int xc = -2; xc <= 2; xc++) {
			for (int zc = -2; zc <= 2; zc++) {
				world.request(xc, i, zc);
			}
		}
	}
}

void Worldgen::scene_ring(World& world) {
	
	Chunk* chunk = world.put(0, 0, 0);

	world.setGenerator( [] (World* world, Chunk* chunk, int cx, int cy, int cz) {
		auto& tree = *chunk->tree;

		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 50; y++) {
				for (int z = 0; z < 64; z++) {
					tree.set(x, y, z, {
						((byte)rand()), ((byte)rand()), ((byte)rand()), 255
					});
				}
			}
		}
	} );

	auto& tree = *chunk->tree;

	tree.set(0, 0, 0, { 255, 0, 0, 255 });
	tree.set(1, 1, 0, { 0, 255, 0, 255 });
	tree.set(2, 2, 0, { 0, 0, 255, 255 });
	tree.set(3, 3, 0, { 255, 255, 0, 255 });
	tree.set(4, 4, 0, { 0, 255, 255, 255 });
	tree.set(5, 5, 0, { 255, 0, 255, 255 });
	tree.set(1, 1, 1, { 100, 255, 100, 255 });

	chunk->markDirty();

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

			world.request(xc, 0, zc);
		}
	}
}
