#pragma once

#include "../config.hpp"

typedef byte chunk_t[64][64][64][4];

class Region;
class Voxel;

namespace Mesh {
	class StaticBuffer;
}

Voxel& get(Voxel* arr, byte x, byte y, byte z);

class Chunk {

	private:
		Voxel* data;
		GLuint vbo, vao;
		size_t size;
		Region* region;
		int cx, cy, cz;

	public:
		Chunk( Voxel*, Region*, int, int, int );
		~Chunk();

		Voxel* xyz(byte x, byte y, byte z);
		void update( Mesh::StaticBuffer* mesh );
		void render( GLuint uniform );
		void discard();
		Mesh::StaticBuffer* build( void* buffer );

		static Voxel* allocate();

		// move this stuff to worldgen.cpp
		static void genBall( Voxel* arr, byte air, int radius  );

};

