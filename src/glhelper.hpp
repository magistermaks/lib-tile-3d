#pragma once

#include "renderer/shader.hpp"
#include "config.hpp"

class ShaderProgram;

namespace GLHelper {

	GLFWwindow* window();

	void vertexAttribute( GLint index, GLint length, GLenum type, GLsizei stride, GLsizei offset, GLsizei size, GLboolean normalize = GL_FALSE );
	void getError( const char* name = nullptr );

	int getSizeOf( GLenum thing );

	bool init(int, int, const char*);
	void frame();

	// screenshot utilities
	byte* capture( int*, int* );
	void screenshot( const std::string& path );

	// platform specific
	cl_context_properties getContext();
	cl_context_properties getDisplay();

	ShaderProgram* loadShaderProgram( std::string );

}

