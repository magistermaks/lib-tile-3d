#pragma once

#include "shader.hpp"
#include "../config.hpp"

// :angry_bread:
class Texture;
class ShaderProgram;
class Charset;
class Screen;
class VertexConsumer;

class RenderSystem {

	private:
		
		ReusableBuffer<float> vertices;
		GLuint vbo, vao, tex;

		VertexConsumer* consumer;
		ShaderProgram* shader;
		GLenum depth;

		RenderSystem();

	public:

		~RenderSystem();

		// singleton pattern
		RenderSystem( RenderSystem& ) = delete;
		void operator=( const RenderSystem& ) = delete;

		// modify renderer state
		void setTexture( Texture& );
		void setShader( ShaderProgram& );
		void setConsumer( VertexConsumer& );
		void setDepthFunc( GLenum depth );

		void vertex3f( float, float, float );
		void vertex2f( float, float, float, float );
		void vertex3f( float, float, float, float, float );

		void depthTest( bool flag );
		void depthMask( bool flag );

		// render specific elements
		void drawText( const std::string&, float, float, float, Charset& );
		void drawScreen( Screen& );

		// draw queued quads
		void draw();

		// wait for opengl
		void flush();

		// get instance of RenderSystem
		static RenderSystem& instance();

};

