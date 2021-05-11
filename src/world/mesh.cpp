
#include "mesh.hpp"

Mesh::ReusableBuffer::ReusableBuffer( size_t length ) {
	this->length = length;
	this->buffer = (byte*) malloc(length);
}

Mesh::ReusableBuffer::ReusableBuffer( ReusableBuffer&& buffer ) {
	this->length = buffer.length;
	this->buffer = buffer.buffer;
}

Mesh::ReusableBuffer::~ReusableBuffer() {
	free(this->buffer);
}
	
inline void Mesh::ReusableBuffer::push( byte data ) {
	this->buffer[ pos ++ ] = data;
}

void Mesh::ReusableBuffer::clear() {
	this->pos = 0;
}

void Mesh::ReusableBuffer::assert_size( int count ) {
	if( this->pos + count > this->length ) {

		if( this->length > LT3D_MAX_MESH_SIZE ) {
			logger::fatal( "Excedded maximum mesh buffer size of " + std::to_string(this->length) + " bytes!" );
			throw std::runtime_error( "Maximum buffer size excedded!" );
		}

		size_t new_size = std::min(this->length * 2, (size_t) LT3D_MAX_MESH_SIZE);

		this->buffer = (byte*) realloc(this->buffer, new_size);
		this->length = new_size;

		logger::warn( "Mesh buffer resized to: " + std::to_string(new_size) + " bytes!" );

	}
}

byte* Mesh::ReusableBuffer::copy() {
	size_t size = this->pos + 1;
	byte* buf = new byte[size];
	memcpy(buf, this->buffer, size);

	return buf;
}

byte* Mesh::ReusableBuffer::data() {
	return this->buffer;
}

size_t Mesh::ReusableBuffer::size() {
	return this->pos + 1;
}

Mesh::StaticBuffer::StaticBuffer( ReusableBuffer* buffer ) : data(buffer->copy()), size(buffer->size()) {
	buffer->clear();
}

Mesh::StaticBuffer::~StaticBuffer() {
	delete[] this->data;
}


void Mesh::buildIndice( ReusableBuffer& vec, byte x, byte y, byte z, const byte* color ) {

	vec.push(x);
	vec.push(y);
	vec.push(z);
	vec.push(color[0]);
	vec.push(color[1]);
	vec.push(color[2]);

}

void Mesh::buildQuad( ReusableBuffer& vec, byte x1, byte y1, byte z1, byte x2, byte y2, byte z2, byte x3, byte y3, byte z3, byte x4, byte y4, byte z4, const byte* color ) {	

#if LT3D_PRIMITIVE == GL_QUADS 
	vec.assert_size( 4*6 );
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
#else
	vec.assert_size( 6*6 );
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
#endif

}

void Mesh::buildVoxel( ReusableBuffer& vec, const byte* color, byte x, byte y, byte z, byte flags ) {

	//                          x   y   z               x   y   z
	//    e-------f      a = ( -1,  1, -1 )  =>  e = ( -1,  1,  1 )
	//   /|      /|      b = (  1,  1, -1 )  =>  f = (  1,  1,  1 )
	//  / |     / |      c = ( -1, -1, -1 )  =>  g = ( -1, -1,  1 )
	// a--|----b  |  =>  d = (  1, -1, -1 )  =>  h = (  1, -1,  1 )
	// |  g----|--h    
	// | /     | /       
	// c-------d 
	//
	// a: ( x - s, y + s, z - s )
	// b: ( x + s, y + s, z - s )
	// c: ( x - s, y - s, z - s )
	// d: ( x + s, y - s, z - s )
	// e: ( x - s, y + s, z + s )
	// f: ( x + s, y + s, z + s )
	// g: ( x - s, y - s, z + s )
	// h: ( x + s, y - s, z + s )

	//                                           1      1      1      2      2      2      3      3      3      4      4      4
	if( flags & 0b100000 ) Mesh::buildQuad( vec, x - 1, y - 1, z + 1, x - 1, y - 1, z - 1, x - 1, y + 1, z + 1, x - 1, y + 1, z - 1, color ); // (g => c => e => a) => -x, left
	if( flags & 0b010000 ) Mesh::buildQuad( vec, x + 1, y - 1, z - 1, x + 1, y - 1, z + 1, x + 1, y + 1, z - 1, x + 1, y + 1, z + 1, color ); // (d => h => b => f) => +x, right
	if( flags & 0b001000 ) Mesh::buildQuad( vec, x - 1, y - 1, z + 1, x + 1, y - 1, z + 1, x - 1, y - 1, z - 1, x + 1, y - 1, z - 1, color ); // (g => h => c => d) => -y, bottom
	if( flags & 0b000100 ) Mesh::buildQuad( vec, x - 1, y + 1, z - 1, x + 1, y + 1, z - 1, x - 1, y + 1, z + 1, x + 1, y + 1, z + 1, color ); // (a => b => e => f) => +y, top
	if( flags & 0b000010 ) Mesh::buildQuad( vec, x - 1, y - 1, z - 1, x + 1, y - 1, z - 1, x - 1, y + 1, z - 1, x + 1, y + 1, z - 1, color ); // (c => d => a => b) => -z, front 
	if( flags & 0b000001 ) Mesh::buildQuad( vec, x + 1, y - 1, z + 1, x - 1, y - 1, z + 1, x + 1, y + 1, z + 1, x - 1, y + 1, z + 1, color ); // (h => g => f => e) => +z, back

}


