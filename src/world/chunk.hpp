#pragma once

#include "../config.hpp"

typedef byte chunk_t[64][64][64][4];

class Region;
class Voxel;

Voxel& get(Voxel* arr, byte x, byte y, byte z);

class Chunk {

	private:
		Voxel* data;
		Region* region;
		int cx, cy, cz;

	public:
		Chunk( Voxel*, Region*, int, int, int );
		~Chunk();

		Voxel* xyz(byte x, byte y, byte z);

		static Voxel* allocate();

		// TODO: move this stuff to worldgen
		static void genBall( Voxel* arr, byte air, int radius  );

};

