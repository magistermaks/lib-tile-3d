#pragma once

#include <GL/glew.h>
#include <vector>
#include <string.h>

#include "logger.hpp"

typedef unsigned char byte;

namespace Mesh {
	std::vector<byte> build( byte*, int ); 
	void buildIndice( std::vector<byte>& vec, byte x, byte y, byte z, byte* color );
	void buildQuad( std::vector<byte>& vec, byte x1, byte y1, byte z1, byte x2, byte y2, byte z2, byte x3, byte y3, byte z3, byte x4, byte y4, byte z4, byte* color );
	void buildVoxel( std::vector<byte>& vec, byte* rgb, byte x, byte y, byte z, byte flags );
}
