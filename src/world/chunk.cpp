
#include "chunk.hpp"

Chunk::Chunk( Region* region, int x, int y, int z ) : cx(x), cy(y), cz(z) {
	this->tree = new VoxelTree(6);
	this->region = region;
}

Chunk::~Chunk() {
	delete[] this->tree;
}

ChunkManager::ChunkManager( PathTracer& tracer ) : tracer(tracer), chunk_size( ((1 - pow(8, (6 + 1))) / -7) * sizeof(VoxelTreeNode) ) {
	this->length = 0;
}

void ChunkManager::realloc( size_t chunk_count ) {
	tracer.resizeVoxels( chunk_count * chunk_size );
}

void ChunkManager::updateMeta() {
	meta.clear();

	for( Chunk* chunk : chunks ) {
		meta.push_back( chunk->cx * 64 );
		meta.push_back( chunk->cy * 64 );
		meta.push_back( chunk->cz * 64 );
	}

	tracer.updateChunks( chunks.size(), meta.data() );
}

void ChunkManager::add( Chunk* chunk ) {
	chunks.push_back( chunk );
	updateMeta();

	if( chunks.size() > length ) {
		this->realloc( chunks.size() );
	}
}

void ChunkManager::remove( Chunk* chunk ) {
	chunks.erase( std::remove( chunks.begin(), chunks.end(), chunk ), chunks.end() );
	updateMeta();
}

void ChunkManager::shrink() {
	if( chunks.size() < length ) {
		this->realloc( chunks.size() );
	}
}

void ChunkManager::clear() {
	this->chunks.clear();
	updateMeta();
}

void ChunkManager::update() {
	int i = 0;

	for( Chunk* chunk : chunks ) {
		if( chunk->tree->dirty() ) {
			tracer.updateVoxels( (i ++) * chunk_size, chunk_size, chunk->tree->data() );
		}
	}
}




