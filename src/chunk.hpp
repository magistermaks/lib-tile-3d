#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "logger.hpp"
#include "mesh.hpp"
#include "glhelper.hpp"

class Chunk {

	private:
		byte* data;
		GLuint vbo, vao;
		size_t size;

	public:
		Chunk( byte* );
		~Chunk();

		void update();
		void render( float x, float y, float z, GLuint uniform );

		// move this stuff to worldgen.cpp
		static void genCube( byte arr[64][64][64][4], byte air );
		static void genBall( byte arr[64][64][64][4], byte air );

};
