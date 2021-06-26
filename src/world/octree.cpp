
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

	VoxelTreeNode** nodes = new VoxelTreeNode*[this->depth];

	// TODO: use some magic here
	for( int i = 1; i <= this->depth; i ++ ) {

		csize /= 2;
		globalid *= 8;

		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 0;
				} else {
					globalid += 3;
					zo += csize;
				}
			} else {

				yo += csize;
				if (z < zo + csize) {
					globalid += 4;
				} else {
					globalid += 7;
					zo += csize;
				}
			}
		} else {
			xo += csize;

			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 1;
				} else {
					globalid += 2;
					zo += csize;
				}
			} else {
				yo += csize;

				if (z < zo + csize) {
					globalid += 5;
				} else {
					globalid += 6;
					zo += csize;
				}
			}
		}

		nodes[i - 1] = buffer + layerindex + globalid;
		pow8 *= 8;
		layerindex += pow8;

	}

	// tree buffer node offset index
	// derived from: `Sn = (1 - q^n) / (1 - q)`
	const int index = (1 - pow(8, (this->depth))) / -7 + globalid;

	// return node pointer
	VoxelTreeNode* node = buffer + index;

	// apply sign changes to affected branches
	if( sign != 0 && (node->empty() ? -1 : +1) != sign ) {
		const int v = sign == -1 ? 1 : 0;

		for( int i = this->depth - 2; i >= 0; i -- ) {
			auto* node = nodes[i];
		
			if( node->a == v ) {
				node->a += sign;
			}else{
				node->a += sign;
				break;
			} 
		}
	}

	delete[] nodes;

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

