#pragma once

#include "shader.hpp"
#include "../config.hpp"

// :angry_bread:
class ShaderProgram;
class Charset;

class Canvas {

	private:

		int width, height;
		GLuint tex;

	public:

		Canvas( int, int );
		~Canvas();

		void update( float* data );
		void bind();
		GLuint id();

		static float* allocate( int, int );

};

class RenderSystem {

	private:
		
		ReusableBuffer<float> vertices;
		GLuint vbo, vao, tex;
		ShaderProgram* shader;

		RenderSystem();

	public:

		~RenderSystem();

		// singleton pattern
		RenderSystem( RenderSystem& ) = delete;
		void operator=( const RenderSystem& ) = delete;

		// modify renderer state
		void setTexture( GLuint );
		void setShader( ShaderProgram& );
		void vertex( float, float, float, float );

		// render specific elements
		void drawText( const std::string&, float, float, float, Charset& );
		void drawScreen( Canvas& );

		// draw queued quads
		void draw();

		// wait for opengl
		void flush();

		// get instance of RenderSystem
		static RenderSystem& instance();

};

