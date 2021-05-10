#pragma once

#include "../config.hpp"

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

	public:
		Chunk( byte*, Region*, int, int, int );
		~Chunk();

		byte* xyz(byte x, byte y, byte z);
		void update( std::vector<byte>* mesh );
		void render( GLuint uniform );
		void discard();
		std::vector<byte>* build();

		static byte* allocate();

		// move this stuff to worldgen.cpp
		static void genCube( byte* arr, byte air );
		static void genBall( byte* arr, byte air, int radius  );

};

