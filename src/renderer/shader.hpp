#pragma once

#include <config.hpp>

class ShaderProgram {

	private:
		GLuint program;

	public:
		ShaderProgram(GLuint);
		~ShaderProgram();

		GLuint id();
		GLuint location(const char*);

		void bind();
};

class ShaderProgramBuilder {

	private:
		std::vector<GLuint> shaders;
		GLint id;

	public:
		bool compileSource( std::string, std::string, GLenum );
		bool compileFile( std::string, GLenum );
		bool link();

		ShaderProgram* build();
};

