
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

void Region::put( byte* chunk, int x, int y, int z ) {
	this->map[ ChunkPos(x, y, z) ] = new Chunk(chunk, this, x, y, z);
}

void Region::remove( int x, int y, int z ) {
	this->map.erase( ChunkPos(x, y, z) );
}

Chunk* Region::chunk( int x, int y, int z ) {
	return this->map.at( ChunkPos(x, y, z) );
}

/* deprecated */
byte* Region::tile( int cx, int cy, int cz, int x, int y, int z ) {

	if( x >= 64 ) {
		cx ++;
		x -= 64;
	}else if( x < 0 ) {
		cx --;
		x += 64;
	}

	if( y >= 64 ) {
		cy ++;
		y -= 64;
	} else if( y < 0 ) {
		cy --;
		y += 64;
	}

	if( z >= 64 ) {
		cz ++;
		z -= 64;
	} else if( z < 0 ) {
		cz --;
		z += 64;
	}

	try{
		return this->chunk( cx, cy, cz )->xyz( x, y, z );
	}catch(...){
		return dummy;
	}

}

void Region::clear() {
	for( auto pair : this->map ) {
		delete pair.second;
	}

	this->map.clear();
}

void Region::render( GLuint location ) {
	for( auto pair : this->map ) {
		pair.second->render( location );
	}
}

void Region::build() {
	for( auto pair : this->map ) {
		pair.second->update();
	}
}


