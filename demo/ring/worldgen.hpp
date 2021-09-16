
void worldgen(World& world) {

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

