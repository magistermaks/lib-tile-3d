#pragma once

#include "config.hpp"

typedef byte chunk_t[64][64][64][4];

class Chunk {

	private:
		byte* data;
		GLuint vbo, vao;
		size_t size;

		std::vector<byte> build();

	public:
		Chunk( byte* );
		~Chunk();

		inline byte* xyz(byte x, byte y, byte z);
		void update();
		void render( float x, float y, float z, GLuint uniform );

		// move this stuff to worldgen.cpp
		static void genCube( chunk_t arr, byte air );
		static void genBall( chunk_t arr, byte air );

};
