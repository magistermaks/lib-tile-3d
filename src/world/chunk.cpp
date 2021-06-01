
#include "chunk.hpp"

Chunk::Chunk( Voxel* data, Region* region, int x, int y, int z ) {
	this->data = data;
	this->region = region;

	this->cx = x;
	this->cy = y;
	this->cz = z;
}

Chunk::~Chunk() {
	delete[] this->data;
}

Voxel* Chunk::allocate() {
	Voxel (*chunk)[64][64];
	chunk = new Voxel[64][64][64];

	return (Voxel*) chunk;
}

//FIXME: UGLY WORLDGEN, MOVE ELSEWHERE

Voxel& get(Voxel* arr, byte x, byte y, byte z) {
	return arr[x * 4096 + y * 64 + z];
}

void Chunk::genBall( Voxel* arr, byte air, int r ) {

	const int c = 32;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				// random RGB
				get(arr, x, y, z) = Voxel::random();

				float A = c - x, B = c - y, C = c - z;

				if( sqrt( A*A + B*B + C*C ) < r ) {
					get(arr, x, y, z).a = ( (byte) rand() ) < air ? 0 : 255;
				}else{
					get(arr, x, y, z).a = 0;
				}

			}
		}
	}
}	
