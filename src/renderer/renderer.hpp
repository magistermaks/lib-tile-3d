#pragma once

#include <core.hpp>

class RenderSystem {

	private:

		VertexConsumer* consumer;
		ShaderProgram* shader;
		Texture* texture;
		GLenum depth;

		RenderSystem();

	public:

		// singleton pattern
		RenderSystem( RenderSystem& ) = delete;
		void operator=( const RenderSystem& ) = delete;

		// modify renderer state
		void setTexture( Texture& );
		void setShader( ShaderProgram& );
		void setConsumer( VertexConsumer& );
		void setDepthFunc( GLenum depth );

		void vertex( float, float );
		void vertex( float, float, float );
		void vertex( float, float, float, float );
		void vertex( float, float, float, float, float );

		void depthTest( bool flag );
		void depthMask( bool flag );

		void clear();

		// render specific elements
		void drawText( const std::string&, float, float, float, Charset& );
		void drawScreen( Screen& );

		// call OpenGL
		void draw();

		// wait for opengl
		void flush();

		// get instance of RenderSystem
		static RenderSystem& instance();

};

