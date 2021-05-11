#pragma once

#include "../config.hpp"

class Chunk;
class Region;

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

	std::string to_string( ChunkPos pos );

}

class ChunkMeshUpdate {

	private:
		ChunkPos pos;
		Mesh::StaticBuffer* data;

	public:
		ChunkMeshUpdate( ChunkPos pos, Mesh::StaticBuffer* data );
		void apply( Region* region );

};

class Region {

	private:
		std::unordered_map< ChunkPos, Chunk* > map;

		ThreadPool pool;
		std::vector<ChunkMeshUpdate> mesh_updates;
		std::mutex mesh_updates_mtx; 

	public:

		Region();

		/// add chunk to region, expects chunk to be on the heap
		void put( byte* chunk, int x, int y, int z );

		/// remove (and free) chunk from region
		void remove( int x, int y, int z );

		/// get chunk from region
		Chunk* chunk( int x, int y, int z );
		Chunk* chunk( ChunkPos& pos );

		/// get tile from world, relative to a chunk
		byte* tile( int cx, int cy, int cz, int x, int y, int z );

		/// remove (and free) all chunks from region
		void clear();

		/// render this region
		void render( GLuint location );

		/// build mesh
		void build();

		/// execute scheduled tasks
		void update();

		/// update chunk mesh
		void update( int x, int y, int z );

		/// Show off thread system
		void discard();

		/// add task to task queue
		void synchronized( ChunkMeshUpdate update );
		

};

