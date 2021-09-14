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

	public:

		using Generator = std::function<void(World*, Chunk*, int, int, int)>;

	private:

		std::unordered_map<ChunkPos, Chunk*> map;
		std::vector<Chunk*> chunks;

		PathTracer& tracer;
		Generator generator;
		
		size_t chunk_size;
		size_t length;

		void notify();

	public:

		World(PathTracer& tracer);

		void setGenerator( const Generator& generator );

		Chunk* put( int x, int y, int z );
		Chunk* get( int x, int y, int z );
		Chunk* request( int x, int y, int z );

		// a little unsafe
		void remove( int x, int y, int z );

		void clear();
		void update();

};

