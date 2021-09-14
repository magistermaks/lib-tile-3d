
#include "chunk.hpp"

Chunk::Chunk( World* world, int x, int y, int z ) : cx(x), cy(y), cz(z) {
	this->tree = new Octree<OctreeVoxel>(6);
	this->world = world;
}

Chunk::~Chunk() {
	delete this->tree;
}

void Chunk::markDirty() {
	this->dirty = true;
}

bool Chunk::readDirty() {
	bool flag = this->dirty;
	this->dirty = false;
	return flag;
}


