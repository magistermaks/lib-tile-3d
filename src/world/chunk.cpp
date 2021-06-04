
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

byte* Chunk::allocate( const int octree_depth ) {
	const int len = (1 - pow(8, (octree_depth + 1))) / -7;
	byte* octree = new byte[len * 4]; //Sn = 1 * (1 - q^n) / (1 - q)

	int index = 0;
	for (int i = 0; i <= octree_depth; i++) {
		int size = std::pow(8, i);
		byte* node = octree + (index * 4); //r g b a
		index += size;
		for (int n = 0; n < size * 4; n += 4) {
			node[n] = 255;
			node[n + 1] = 255;
			node[n + 2] = 255;
			node[n + 3] = 255;
		}
	}


	const int csize = std::pow(2, octree_depth);
	const int cm = 256 / csize;
	for (int x = 0; x < csize; x++)
		for (int y = 0; y < csize; y++)
			for (int z = 0; z < csize; z++)
				set_voxel(x, y, z, octree, octree_depth, x * cm, y * cm, z * cm, csize);

	return octree;
}

//FIXME: UGLY WORLDGEN, MOVE ELSEWHERE
void Chunk::set_voxel(int x, int y, int z, byte* octree, int octree_depth, byte r, byte g, byte b, int csize) {
	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	int layerindex = 1;
	for (int depth = 1; depth <= octree_depth; depth++) {
		csize /= 2;
		octree[layerindex + globalid * 4 + 3] = 255;
		layerindex += pow(8, depth - 1);
		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid = globalid * 8 + 0;
				}
				else {
					globalid = globalid * 8 + 3;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid = globalid * 8 + 4;
				}
				else {
					globalid = globalid * 8 + 7;
					zo += csize;
				}
			}
		}
		else {
			xo += csize;
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid = globalid * 8 + 1;
				}
				else {
					globalid = globalid * 8 + 2;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid = globalid * 8 + 5;
				}
				else {
					globalid = globalid * 8 + 6;
					zo += csize;
				}
			}
		}

	}
	const int index = ((1 - pow(8, (octree_depth))) / -7 + globalid - 1) * 4;
	octree[index] = r;
	octree[index + 1] = g;
	octree[index + 2] = b;
	octree[index + 3] = 255;
}

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
