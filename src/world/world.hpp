#pragma once

#include <core.hpp>

struct ChunkPos {

	const int x, y, z;

	ChunkPos( const int x, const int y, const int z );

	bool operator<(const ChunkPos& pos) const;
	bool operator==(const ChunkPos& pos) const;

};

// std::hash specialization for ChunkPos
namespace std {

	template <>
	struct hash<ChunkPos> : public std::function<std::size_t(const ChunkPos&)> {
		inline std::size_t operator()( const ChunkPos& pos ) const;
	};

	std::string to_string( const ChunkPos& pos );

}

class World {

	private:

		std::unordered_map<ChunkPos, Chunk*> map;
		std::vector<Chunk*> chunks;

		PathTracer& tracer;
		
		size_t chunk_size;
		size_t length;

		void notify();

	public:

		World(PathTracer& tracer);

		void put( int x, int y, int z );
		void put( const ChunkPos& pos );
	
		void remove( int x, int y, int z );

		// get chunk from region
		Chunk* get( int x, int y, int z );
		Chunk* get( const ChunkPos& pos );

		// remove all chunks from region
		void clear();

		// submit changes to renderer
		void update();

};

