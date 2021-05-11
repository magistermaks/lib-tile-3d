
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

std::string std::to_string( ChunkPos pos ) {
	return std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);
}

ChunkMeshUpdate::ChunkMeshUpdate( ChunkPos xyz, Mesh::StaticBuffer* data ) : pos( xyz ) {
	this->data = data;
}

void ChunkMeshUpdate::apply( Region* region ) {
	region->chunk( this->pos )->update( this->data );
}

Region::Region() : pool() {
	int threads = ThreadPool::optimal();

	while( threads --> 0 ) {
		pool.addWorker<Mesh::ReusableBuffer>( []() { return Mesh::ReusableBuffer( 15000 ); } );
	}
}

void Region::put( byte* chunk, int x, int y, int z ) {
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
		//throw std::runtime_error( "No chunk at: " + std::to_string(pos) );
		return nullptr;
	}
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
		this->update( pair.first.x, pair.first.y, pair.first.z );
	}
}

void Region::update( int x, int y, int z ) {
	ChunkPos pos( x, y, z );
	Chunk* chunk = this->chunk( pos );

	this->pool.enqueue( [this, chunk, pos](void* buffer) {
		this->synchronized( ChunkMeshUpdate( pos, chunk->build( buffer ) ) );
	} );
}

void Region::discard() {
	for( auto pair : this->map ) {
		pair.second->discard();
	}
}

void Region::update() {
	if( !this->mesh_updates.empty() ) {
		this->mesh_updates_mtx.lock();

		for( auto& update : this->mesh_updates ) {
			update.apply(this);
		}

		this->mesh_updates.clear();
		this->mesh_updates_mtx.unlock();
	}
}

void Region::synchronized( ChunkMeshUpdate update ) {
	this->mesh_updates_mtx.lock();
	this->mesh_updates.push_back(update);
	this->mesh_updates_mtx.unlock();
}


