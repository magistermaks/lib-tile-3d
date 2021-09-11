#pragma once

#include <core.hpp>

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

