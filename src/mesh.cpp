#include "mesh.hpp"

void Mesh::buildIndice( std::vector<byte>& vec, byte x, byte y, byte z, byte* color ) {

	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	vec.push_back(color[0]);
	vec.push_back(color[1]);
	vec.push_back(color[2]);

}

void Mesh::buildQuad( std::vector<byte>& vec, byte x1, byte y1, byte z1, byte x2, byte y2, byte z2, byte x3, byte y3, byte z3, byte x4, byte y4, byte z4, byte* color ) {	

#if LT3D_PRIMITIVE == GL_QUADS 
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
#else
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
#endif

}

void Mesh::buildVoxel( std::vector<byte>& vec, byte* color, byte x, byte y, byte z, byte flags ) {

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


