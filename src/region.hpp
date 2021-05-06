#pragma once

#include "config.hpp"

// it may be beneficial to use https://github.com/greg7mdp/parallel-hashmap instead of the C++ ones

class Chunk;
class DummyChunk;

struct ChunkPos {
	int x, y, z;

	ChunkPos( int x, int y, int z );

	bool operator<(const ChunkPos& pos) const;
	bool operator==(const ChunkPos& pos) const;
};

namespace std {

	template <>
	struct hash<ChunkPos> : public std::unary_function<const ChunkPos&, std::size_t> {
		inline std::size_t operator()( const ChunkPos& pos ) const;
	};

}

class Region {

	private:
		std::unordered_map< ChunkPos, Chunk* > map;

	public:

		/// add chunk to region, expects chunk to be on the heap
		void put( byte* chunk, int x, int y, int z );

		/// remove (and free) chunk from region
		void remove( int x, int y, int z );

		/// get chunk from region
		Chunk* chunk( int x, int y, int z );

		/// get tile from world, relative to a chunk
		byte* tile( int cx, int cy, int cz, int x, int y, int z );

		/// remove (and free) all chunks from region
		void clear();

		/// render this region
		void render( GLuint location );

		/// build mesh
		void build();
		

};

