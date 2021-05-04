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

void Mesh::buildVoxel( std::vector<GLfloat>& vec, byte* rgb, float x, float y, float z, float s ) {

	const float color = Mesh::buildFakeFloat(rgb[0], rgb[1], rgb[2], 255);

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
	Mesh::buildQuad( vec, x - s, y - s, z - s, x + s, y - s, z - s, x - s, y + s, z - s, x + s, y + s, z - s, color ); // (c => d => a => b) => front 
	Mesh::buildQuad( vec, x + s, y - s, z + s, x - s, y - s, z + s, x + s, y + s, z + s, x - s, y + s, z + s, color ); // (h => g => f => e) => back
	Mesh::buildQuad( vec, x - s, y - s, z + s, x - s, y - s, z - s, x - s, y + s, z + s, x - s, y + s, z - s, color ); // (g => c => e => a) => left
	Mesh::buildQuad( vec, x + s, y - s, z - s, x + s, y - s, z + s, x + s, y + s, z - s, x + s, y + s, z + s, color ); // (d => h => b => f) => right
	Mesh::buildQuad( vec, x - s, y + s, z - s, x + s, y + s, z - s, x - s, y + s, z + s, x + s, y + s, z + s, color ); // (a => b => e => f) => top
	Mesh::buildQuad( vec, x - s, y - s, z + s, x + s, y - s, z + s, x - s, y - s, z - s, x + s, y - s, z - s, color ); // (g => h => c => d) => bottom
		
}

std::vector<GLfloat> Mesh::build( byte* arr, int size ) {

	std::vector<GLfloat> vertex_buffer;

	logger::info("Generating vertex data...");

	const float s = 1.f / size;
	const int zoff = 3;
	const int yoff = zoff * size;
	const int xoff = yoff * size;

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {
				const float xs = ((float) x / size) * 2 - 1.f + s;
				const float ys = ((float) y / size) * 2 - 1.f + s;
				const float zs = ((float) z / size) * 2 - 1.f + s;

				Mesh::buildVoxel( vertex_buffer, &(arr[x * xoff + y * yoff + z * zoff]), xs, ys, zs, s );
			}
		}
	}

	vertex_buffer.shrink_to_fit();
	return vertex_buffer;

}


