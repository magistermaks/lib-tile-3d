#include "mesh.hpp"

float Mesh::buildFakeFloat( byte a, byte b, byte c, byte d ) {

	GLfloat value;
	byte* facs = (byte*) &value;

	facs[0] = a;
	facs[1] = b;
	facs[2] = c;
	facs[3] = d;

	return value;
}

void Mesh::buildIndice( std::vector<GLfloat>& vec, GLfloat x, GLfloat y, GLfloat z, GLfloat color ) {
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	vec.push_back(color);
}

void Mesh::buildQuad( std::vector<GLfloat>& vec, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4, GLfloat color ) {
	Mesh::buildIndice( vec, x1, y1, z1, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
	Mesh::buildIndice( vec, x2, y2, z2, color );
	Mesh::buildIndice( vec, x4, y4, z4, color );
	Mesh::buildIndice( vec, x3, y3, z3, color );
}

void Mesh::buildVoxel( std::vector<GLfloat>& vec, byte* rgb, float x, float y, float z, float s, byte flags ) {

	const float color = Mesh::buildFakeFloat(rgb[0], rgb[1], rgb[2], rgb[3]);

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
	//                    1      1      1      2      2      2      3      3      3      4      4      4
	if( flags & 0b000010 ) Mesh::buildQuad( vec, x - s, y - s, z - s, x + s, y - s, z - s, x - s, y + s, z - s, x + s, y + s, z - s, color ); // (c => d => a => b) => -z, front 
	if( flags & 0b000001 ) Mesh::buildQuad( vec, x + s, y - s, z + s, x - s, y - s, z + s, x + s, y + s, z + s, x - s, y + s, z + s, color ); // (h => g => f => e) => +z, back
	if( flags & 0b100000 ) Mesh::buildQuad( vec, x - s, y - s, z + s, x - s, y - s, z - s, x - s, y + s, z + s, x - s, y + s, z - s, color ); // (g => c => e => a) => -x, left
	if( flags & 0b010000 ) Mesh::buildQuad( vec, x + s, y - s, z - s, x + s, y - s, z + s, x + s, y + s, z - s, x + s, y + s, z + s, color ); // (d => h => b => f) => +x, right
	if( flags & 0b001000 ) Mesh::buildQuad( vec, x - s, y - s, z + s, x + s, y - s, z + s, x - s, y - s, z - s, x + s, y - s, z - s, color ); // (g => h => c => d) => -y, bottom
	if( flags & 0b000100 ) Mesh::buildQuad( vec, x - s, y + s, z - s, x + s, y + s, z - s, x - s, y + s, z + s, x + s, y + s, z + s, color ); // (a => b => e => f) => +y, top
		
}

#define XYZ( x, y, z ) (&(arr[(x) * xoff + (y) * yoff + (z) * zoff]))

std::vector<GLfloat> Mesh::build( byte* arr, int size ) {

	std::vector<GLfloat> vertex_buffer;

	logger::info("Generating vertex data...");

	const float s = 1.f / size;
	const int zoff = 4;
	const int yoff = zoff * size;
	const int xoff = yoff * size;
	const int m = size - 1;

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {
				const float xs = ((float) x / size) * 2 - 1.f + s;
				const float ys = ((float) y / size) * 2 - 1.f + s;
				const float zs = ((float) z / size) * 2 - 1.f + s;

				byte* vox = XYZ(x, y, z);
				
				if( vox[3] != 0 ) {

					byte flags = 0;
					flags |= (x == 0 || (XYZ(x - 1, y, z)[3] != 255)) ? 0b100000 : 0; // -X
					flags |= (x == m || (XYZ(x + 1, y, z)[3] != 255)) ? 0b010000 : 0; // +X
					flags |= (y == 0 || (XYZ(x, y - 1, z)[3] != 255)) ? 0b001000 : 0; // -Y
					flags |= (y == m || (XYZ(x, y + 1, z)[3] != 255)) ? 0b000100 : 0; // +Y
					flags |= (z == 0 || (XYZ(x, y, z - 1)[3] != 255)) ? 0b000010 : 0; // -Z
					flags |= (z == m || (XYZ(x, y, z + 1)[3] != 255)) ? 0b000001 : 0; // +Z

					if( flags ) Mesh::buildVoxel( vertex_buffer, XYZ(x, y, z), xs, ys, zs, s, flags );

				}
			}
		}
	}

	vertex_buffer.shrink_to_fit();
	return vertex_buffer;

}


