
#include "octree.hpp"

VoxelTree::VoxelTree( const int depth ) : depth(depth), size( std::pow(2, depth) ) {

	// length of the buffer used to store the tree
	// derived from: `Sn = (1 - q^n) / (1 - q)`
	const int length = (1 - pow(8, (depth + 1))) / -7;

	// allocate the buffer
	this->buffer = new VoxelTreeNode[length];

	// clear allocated tree buffer
	memset(this->buffer, 0, length * sizeof(VoxelTreeNode));

	// update range
	this->modified = true;

}

VoxelTree::~VoxelTree() {
	delete[] this->buffer;
}

VoxelTreeNode* VoxelTree::get( const int x, const int y, const int z, const int sign ) {

	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	int layerindex = 1;
	int pow8 = 1;
	int csize = size;

	// TODO: use some magic here
	for( int i = 1; i <= this->depth; i ++ ) {

		csize /= 2;
		int oc = 0;

		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					oc = 0;
				} else {
					oc = 3;
					zo += csize;
				}
			} else {

				yo += csize;
				if (z < zo + csize) {
					oc = 4;
				} else {
					oc = 7;
					zo += csize;
				}
			}
		} else {
			xo += csize;

			if (y < yo + csize) {
				if (z < zo + csize) {
					oc = 1;
				} else {
					oc = 2;
					zo += csize;
				}
			} else {
				yo += csize;

				if (z < zo + csize) {
					oc = 5;
				} else {
					oc = 6;
					zo += csize;
				}
			}
		}

		// indicate child presence
		byte* node_a = &this->buffer[(layerindex - pow8 + globalid)].a;
		if (sign >= 1)
			*node_a |= 1UL << oc;

		globalid = globalid * 8 + oc;
		pow8 *= 8;
		layerindex += pow8;

		if (sign <= -1 && this->buffer[(layerindex - pow8 + globalid)].a == ~(1UL << oc))
			*node_a &= ~(1UL << oc);
	}

	// tree buffer node offset index
	// derived from: `Sn = (1 - q^n) / (1 - q)`
	const int index = (1 - pow(8, (this->depth))) / -7 + globalid;

	// return node pointer
	VoxelTreeNode* node = buffer + index;

	// return node pointer
	return node;
}

void VoxelTree::set( const int x, const int y, const int z, const VoxelTreeNode& node ) {

	VoxelTreeNode* ptr = get(x, y, z, node.empty() ? -1 : +1);

	// copy given node into the tree
	memcpy( ptr, &node, sizeof(VoxelTreeNode) );

	this->modified = true;

}

bool VoxelTree::dirty() {
	bool flag = this->modified;
	this->modified = false;
	return flag;
}

byte* VoxelTree::data() {
	return (byte*) this->buffer;
}

