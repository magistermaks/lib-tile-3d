#pragma once

#include "../config.hpp"

// this must be a simple, C-like structure, bc we do some memory magic on it
struct VoxelTreeNode {

	bool empty() const {
		return a == 0;
	}

	byte r; // red color
	byte g; // green color
	byte b; // blue color
	byte a; // translucency 
//	byte i; // light intensity

};

class VoxelTree {

	private:

		// holds the tree structure
		VoxelTreeNode* buffer;

		const int depth, size;
		bool modified;

		// mark a spot in the tree as dirty (modified)
		void mark( size_t pos );
		void unmark();
	
	public:

		VoxelTree( const int depth );
		~VoxelTree();

		// get voxel pointer from tree
		VoxelTreeNode* get( const int x, const int y, const int z, const int sign = 0 );

		// set voxel in tree
		void set( const int x, const int y, const int z, const VoxelTreeNode& node );

		// data getters
		bool dirty();
		byte* data();

};

