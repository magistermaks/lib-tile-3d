
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
		byte* node = octree + index; //r g b a
		index += size * 4;
		for (int n = 0; n < size * 4; n += 4) {
			node[n] = 255;
			node[n + 1] = 255;
			node[n + 2] = 255;
			node[n + 3] = 0;
		}
	}


	const int csize = std::pow(2, octree_depth);
	const int cm = (int)(256.0f / (float)csize);
	const int c = csize / 2;
	for (int x = 0; x < csize; x++)
		for (int y = 0; y < csize; y++)
			for (int z = 0; z < csize; z++) {
				float A = c - x, B = c - y, C = c - z;
				//sqrt(A * A + B * B + C * C) < 6) { sphere
				if (y < (sin(x * 0.5f) + 1.0f) * c && y < (cos(z * 0.5f) + 1.0f) * c){
					set_voxel(x, y, z, octree, octree_depth, x * cm, y * cm, z * cm, csize);
					//set_voxel(x, y, z, octree, octree_depth, (float)x * (sin(x * 0.25f) + 1.0f), (float)y * (sin(y * 0.25f) + 1.0f), (float)z * (sin(z * 0.25f) + 1.0f), csize);
				}
			}

	return octree;
}

//FIXME: UGLY WORLDGEN, MOVE ELSEWHERE
void Chunk::set_voxel(int x, int y, int z, byte* octree, int octree_depth, byte r, byte g, byte b, int csize) {
	int globalid = 0;
	int xo = 0, yo = 0, zo = 0;
	int layerindex = 1;
	int pow8 = 1;
	for (int depth = 1; depth <= octree_depth; depth++) {
		csize /= 2;

		globalid *= 8;

		if (x < xo + csize) {
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 0;
				}
				else {
					globalid += 3;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid += 4;
				}
				else {
					globalid += 7;
					zo += csize;
				}
			}
		}
		else {
			xo += csize;
			if (y < yo + csize) {
				if (z < zo + csize) {
					globalid += 1;
				}
				else {
					globalid += 2;
					zo += csize;
				}
			}
			else {
				yo += csize;
				if (z < zo + csize) {
					globalid += 5;
				}
				else {
					globalid += 6;
					zo += csize;
				}
			}
		}

		int a = (layerindex + globalid) * 4;
		octree[a + 3] = 255;
		pow8 *= 8;
		layerindex += pow8;

	}
	const int index = ((1 - pow(8, (octree_depth))) / -7 + globalid) * 4;
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

	for ( int x = 0; x < 64; x ++ ) {
		for ( int y = 0; y < 64; y ++ ) {
			for ( int z = 0; z < 64; z ++ ) {

				// random RGB
				get(arr, x, y, z) = Voxel::random();

				float A = c - x, B = c - y, C = c - z;

				if ( sqrt( A * A + B * B + C * C ) < r ) {
					get(arr, x, y, z).a = ( (byte) rand() ) < air ? 0 : 255;
				}
				else {
					get(arr, x, y, z).a = 0;
				}

			}
		}
	}
}
