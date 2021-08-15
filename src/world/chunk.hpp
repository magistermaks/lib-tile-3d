#pragma once

#include "octree.hpp"
#include "../config.hpp"

typedef byte chunk_t[64][64][64][4];

class Region;
class PathTracer;
class OctreeVoxel;

template< typename T >
class Octree;

class Chunk {

	private:

		Region* region;

	public:

		// TODO: make this private
		Octree<OctreeVoxel>* tree;
		const int cx, cy, cz;

		Chunk( Region*, int, int, int );
		~Chunk();

};

class ChunkManager {

	private:

		const size_t chunk_size;

		std::vector<Chunk*> chunks;
		std::vector<float> meta;

		size_t length;

		PathTracer& tracer;

		void realloc( size_t chunk_count );
		void updateMeta();

	public:

		ChunkManager( PathTracer& tracer );

		void add( Chunk* chunk );
		void remove( Chunk* chunk );
		void shrink();
		void update();
		void clear();

};

