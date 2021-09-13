
#include "world.hpp"

ChunkPos::ChunkPos( const int x, const int y, const int z ) : x(x), y(y), z(z) {
	
}

bool ChunkPos::operator<(const ChunkPos& pos) const {
	return std::tie(x, y, z) < std::tie(pos.x, pos.y, pos.z);
}

bool ChunkPos::operator==(const ChunkPos& pos) const {
	return x == pos.x && y == pos.y && z == pos.z;
}

std::size_t std::hash<ChunkPos>::operator()( const ChunkPos& pos ) const {
	return (pos.z & 0x0FF) | (pos.y & 0xFFF) << 2 | (pos.x & 0xFFF) << 5;
}

std::string std::to_string( const ChunkPos& pos ) {
	return std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);
}

World::World(PathTracer& tracer) : tracer(tracer), length(0), chunk_size( Octree<OctreeVoxel>::sizeOf(6) * sizeof(OctreeVoxel) ) {
	
}

void World::notify() {
	std::vector<float> offsets;

	for( Chunk* chunk : this->chunks ) {
		offsets.push_back( chunk->cx * 64 );
		offsets.push_back( chunk->cy * 64 );
		offsets.push_back( chunk->cz * 64 );
	}

	this->tracer.updateChunks( chunks.size(), offsets.data() );
}

void World::put( int x, int y, int z ) {
	this->put( ChunkPos(x, y, z) );
}

void World::put( const ChunkPos& pos ) {
	Chunk* chunk = new Chunk(this, pos.x, pos.y, pos.z);

	// add chunk to buffers
	this->map[pos] = chunk;
	this->chunks.push_back(chunk);

	// if there are more chunks than slots in kernel buffers
	// resize kernel chunk buffers
	const size_t chunk_count = chunks.size();

	if( chunk_count > length ) {

		this->tracer.resizeVoxels( chunk_count * chunk_size );
		length = chunk_count;
	}

	// update kernel buffers
	this->notify();

	logger::info("Added chunk #", chunk_count, " at: (", std::to_string(pos), ")");
}

void World::remove( int x, int y, int z ) {
	ChunkPos pos = ChunkPos(x, y, z);
	Chunk* chunk = this->map[pos];

	// remove chunk from buffers
	this->chunks.erase( std::remove( chunks.begin(), chunks.end(), chunk ), chunks.end() );
	this->map.erase(pos);

	// update kernel buffers
	this->notify();
}

Chunk* World::get( int x, int y, int z ) {
	return this->get( ChunkPos(x, y, z) );
}

Chunk* World::get( const ChunkPos& pos ) {
	try {
		return this->map.at( pos );
	} catch(std::out_of_range& err) {
		return nullptr;
	}
}

void World::clear() {
	for( auto pair : this->map ) {
		delete pair.second;
	}

	this->map.clear();
	this->chunks.clear();

	this->length = 0;
	
	// update kernel buffers
	this->notify();
}

void World::update() {
	int i = 0;

	for( Chunk* chunk : chunks ) {
		if( chunk->tree->dirty() ) {
			tracer.updateVoxels( (i ++) * chunk_size, chunk_size, chunk->tree->data() );
		}
	}
}

