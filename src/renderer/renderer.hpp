#pragma once

#include "../config.hpp"

// :angry_bread:
namespace GLHelper { class ShaderProgram; }
class Charset;

class Canvas {

	private:

		int width, height;
		bool fresh;
		GLuint tex;

	public:

		Canvas( int, int );
		~Canvas();

		void update( byte* data );
		void bind();
		GLuint id();

		static byte* allocate( int, int );

};

class RenderSystem {

	private:
		
		ReusableBuffer<float> vertices;
		GLuint vbo, vao, tex;
		GLHelper::ShaderProgram* shader;

		RenderSystem();

	public:

		~RenderSystem();

		// singleton pattern
		RenderSystem( RenderSystem& ) = delete;
		void operator=( const RenderSystem& ) = delete;

		// modify renderer state
		void setTexture( GLuint );
		void setShader( GLHelper::ShaderProgram& );
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

