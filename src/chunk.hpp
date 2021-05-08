#pragma once

#include "config.hpp"

typedef byte chunk_t[64][64][64][4];

class Region;

byte* get(byte* arr, byte x, byte y, byte z);

class Chunk {

	private:
		byte* data;
		GLuint vbo, vao;
		size_t size;
		Region* region;
		int cx, cy, cz;

		std::vector<byte> build();

	public:
		Chunk( byte*, Region*, int, int, int );
		~Chunk();

		byte* xyz(byte x, byte y, byte z);
		void update();
		void render( GLuint uniform );

		static byte* allocate();

		// move this stuff to worldgen.cpp
		static void genCube( byte* arr, byte air );
		static void genBall( byte* arr, byte air, int radius  );

};

