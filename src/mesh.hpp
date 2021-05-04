#pragma once

#include <GL/glew.h>
#include <vector>
#include <string.h>

#include "logger.hpp"

typedef unsigned char byte;

namespace Mesh {
	std::vector<GLfloat> build( byte*, int ); 
	float buildFakeFloat( byte, byte, byte, byte );
	void buildIndice( std::vector<GLfloat>& vec, GLfloat x, GLfloat y, GLfloat z, GLfloat color );
	void buildQuad( std::vector<GLfloat>& vec, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4, GLfloat color );
	void buildVoxel( std::vector<GLfloat>& vec, byte* rgb, float x, float y, float z, float s );
}
