#include "mesh.hpp"

void Mesh::buildIndice( std::vector<byte>& vec, byte x, byte y, byte z, byte* color ) {
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	vec.push_back(color[0]);
	vec.push_back(color[1]);
	vec.push_back(color[2]);
	vec.push_back(color[3]);
}

void Mesh::buildQuad( std::vector<byte>& vec, byte x1, byte y1, byte z1, byte x2, byte y2, byte z2, byte x3, byte y3, byte z3, byte x4, byte y4, byte z4, byte* color ) {
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
}

void Mesh::buildVoxel( std::vector<byte>& vec, byte* color, byte x, byte y, byte z, byte flags ) {

	const byte s = 1;

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
	//
	//                                           1      1      1      2      2      2      3      3      3      4      4      4
	if( flags & 0b000010 ) Mesh::buildQuad( vec, x - s, y - s, z - s, x + s, y - s, z - s, x - s, y + s, z - s, x + s, y + s, z - s, color ); // (c => d => a => b) => -z, front 
	if( flags & 0b000001 ) Mesh::buildQuad( vec, x + s, y - s, z + s, x - s, y - s, z + s, x + s, y + s, z + s, x - s, y + s, z + s, color ); // (h => g => f => e) => +z, back
	if( flags & 0b100000 ) Mesh::buildQuad( vec, x - s, y - s, z + s, x - s, y - s, z - s, x - s, y + s, z + s, x - s, y + s, z - s, color ); // (g => c => e => a) => -x, left
	if( flags & 0b010000 ) Mesh::buildQuad( vec, x + s, y - s, z - s, x + s, y - s, z + s, x + s, y + s, z - s, x + s, y + s, z + s, color ); // (d => h => b => f) => +x, right
	if( flags & 0b001000 ) Mesh::buildQuad( vec, x - s, y - s, z + s, x + s, y - s, z + s, x - s, y - s, z - s, x + s, y - s, z - s, color ); // (g => h => c => d) => -y, bottom
	if( flags & 0b000100 ) Mesh::buildQuad( vec, x - s, y + s, z - s, x + s, y + s, z - s, x - s, y + s, z + s, x + s, y + s, z + s, color ); // (a => b => e => f) => +y, top
		
}

#define XYZ( x, y, z ) (&(arr[(x) * xoff + (y) * yoff + (z) * zoff]))

std::vector<byte> Mesh::build( byte* arr, int size ) {

	std::vector<byte> vertex_buffer;

	const int zoff = 4;
	const int yoff = zoff * size;
	const int xoff = yoff * size;
	const int m = size - 1;

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {
				byte* vox = XYZ(x, y, z);
				
				if( vox[3] != 0 ) {

					byte flags = 0;
					flags |= (x == 0 || (XYZ(x - 1, y, z)[3] != 255)) ? 0b100000 : 0; // -X
					flags |= (x == m || (XYZ(x + 1, y, z)[3] != 255)) ? 0b010000 : 0; // +X
					flags |= (y == 0 || (XYZ(x, y - 1, z)[3] != 255)) ? 0b001000 : 0; // -Y
					flags |= (y == m || (XYZ(x, y + 1, z)[3] != 255)) ? 0b000100 : 0; // +Y
					flags |= (z == 0 || (XYZ(x, y, z - 1)[3] != 255)) ? 0b000010 : 0; // -Z
					flags |= (z == m || (XYZ(x, y, z + 1)[3] != 255)) ? 0b000001 : 0; // +Z

					if( flags ) Mesh::buildVoxel( vertex_buffer, XYZ(x, y, z), x*2, y*2, z*2, flags );

				}
			}
		}
	}

	vertex_buffer.shrink_to_fit();
	return vertex_buffer;

}


