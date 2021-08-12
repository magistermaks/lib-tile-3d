#pragma once

#include "../config.hpp"

class Chunk;
class Region;
class ChunkManager;
	
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

	std::string to_string( const ChunkPos& pos );

}

class Region {

	private:
		std::unordered_map< ChunkPos, Chunk* > map;
		ChunkManager& manager;

	public:

		Region( ChunkManager& );

		/// add chunk to region, expects chunk to be on the heap
		void put( Voxel* chunk, int x, int y, int z );

		/// remove (and free) chunk from region
		void remove( int x, int y, int z );

		/// get chunk from region
		Chunk* chunk( int x, int y, int z );
		Chunk* chunk( ChunkPos& pos );

		/// remove (and free) all chunks from region
		void clear();


};

