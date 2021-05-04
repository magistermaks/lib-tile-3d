#pragma once

#include <GL/glew.h>
#include <vector>

#include "logger.hpp"

typedef unsigned char byte;

namespace Mesh {
    std::vector<GLfloat> build( byte*, int ); 
    void buildIndice( std::vector<GLfloat>& vec, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b );
    void buildQuad( std::vector<GLfloat>& vec, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4, GLfloat r, GLfloat g, GLfloat b );
    void buildVoxel( std::vector<GLfloat>& vec, byte* rgb, float x, float y, float z, float s );
}
