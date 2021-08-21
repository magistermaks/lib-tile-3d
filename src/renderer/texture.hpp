#pragma once

#include "../config.hpp"

class Texture {

	private:

		int width, height;
		GLenum format, type;
		GLuint tex;

	public:

		Texture( int width, int height, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, GLint interpolation = GL_NEAREST );
		~Texture();

		void resize( int width, int height );

		void bind();
		GLuint id();

	public:
		
		template< typename T >
		void update( T* data ) {
			this->bind();
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, this->width, this->height, this->format, this->type, (void*) data );
		}

};

class Screen : public Texture {
	
	public:
		Screen( int width, int height );

};

