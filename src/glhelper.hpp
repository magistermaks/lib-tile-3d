#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <vector>
#include <fstream>

#include "logger.hpp"

namespace GLHelper {

	GLFWwindow* window();

	void vertexAttribute( GLint index, GLint length, GLenum type, GLsizei stride, GLsizei offset, GLsizei size );

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
	ShaderProgram loadShaders();

}

