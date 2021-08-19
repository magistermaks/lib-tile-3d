#pragma once

#include "config.hpp"

byte randomByte();

namespace GLHelper {

	GLFWwindow* window();

	void vertexAttribute( GLint index, GLint length, GLenum type, GLsizei stride, GLsizei offset, GLsizei size, GLboolean normalize = GL_FALSE );
	void getError( const char* name = nullptr );

	class ShaderProgramBuilder {

		private:
			std::vector<GLuint> shaders;
			bool failed = false;
			GLint id;

		public:
			void compileSource( std::string, std::string, GLenum );
			void compileFile( std::string, GLenum );
			void link();

			bool isOk();
			GLint get();
	};

	class ShaderProgram {

		private:
			GLuint program;

		public:
			ShaderProgram(GLuint);
			ShaderProgram(ShaderProgramBuilder&);
			~ShaderProgram();

			GLuint id();
			GLuint location(const char*);
			void bind();
			static ShaderProgramBuilder make();
	};

	bool init(int, int, const char*);
	void frame();
	byte* capture( int*, int* );
	void screenshot( const char* path );

	// platform specific
	cl_context_properties getContext();
	cl_context_properties getDisplay();

	ShaderProgram loadShaders( std::string );

}

