#pragma once

#include <core.hpp>

class Chunk {

	private:

		World* world;

	public:

		Octree<OctreeVoxel>* tree;
		const int cx, cy, cz;

		Chunk( World*, int, int, int );
		~Chunk();

};

