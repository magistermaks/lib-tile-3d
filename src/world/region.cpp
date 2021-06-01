
#include "region.hpp"

byte dummy[4] = {0, 0, 0, 0};

ChunkPos::ChunkPos( int x, int y, int z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

bool ChunkPos::operator<(const ChunkPos& pos) const {
	return std::tie(x, y, z) < std::tie(pos.x, pos.y, pos.z);
}

bool ChunkPos::operator==(const ChunkPos& pos) const {
	return (x == pos.x && y == pos.y && z == pos.z);
}

std::size_t std::hash<ChunkPos>::operator()( const ChunkPos& pos ) const {
	return murmur3_32( (uint8_t*) &pos, sizeof(ChunkPos), 0 );
}

std::string std::to_string( const ChunkPos& pos ) {
	return std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);
}

void Region::put( Voxel* chunk, int x, int y, int z ) {
	this->map[ ChunkPos(x, y, z) ] = new Chunk(chunk, this, x, y, z);
}

void Region::remove( int x, int y, int z ) {
	this->map.erase( ChunkPos(x, y, z) );
}

Chunk* Region::chunk( int x, int y, int z ) {
	ChunkPos pos( x, y, z );
	return this->chunk( pos );
}

Chunk* Region::chunk( ChunkPos& pos ) {
	try {
		return this->map.at( pos );
	} catch(std::out_of_range& err) {
		return nullptr;
	}
}

void Region::clear() {
	for( auto pair : this->map ) {
		delete pair.second;
	}

	this->map.clear();
}

