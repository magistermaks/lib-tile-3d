
#include "texture.hpp"

Texture::Texture( int width, int height, GLenum format, GLenum internal, GLenum type, GLint interpolation ) {
	this->width = width;
	this->height = height;
	this->format = format;
	this->internal = internal;
	this->type = type;

	glGenTextures(1, &tex);  

	// set texture settings
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

	// initialize empty texture
	this->resize( width, height );
}

Texture::~Texture() {
	 glDeleteTextures(1, &tex);
}

void Texture::resize( int width, int height ) {
	glTexImage2D( GL_TEXTURE_2D, 0, this->internal, width, height, 0, this->format, this->type, nullptr );
}

void Texture::bind() {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, this->tex );
}

GLuint Texture::id() {
	return this->tex;
}

cl::Image2DGL Texture::getHandle( cl_mem_flags flags ) {
	return cl::Image2DGL( cl::Context::getDefault(), flags, GL_TEXTURE_2D, 0, this->tex );
}

int Texture::getWidth() {
	return this->width;
}

int Texture::getHeight() {
	return this->height;
}

Texture* Texture::fromFile( const char* path ) {

	int w, h, n;
	byte* data = stbi_load(path, &w, &h, &n, 4);

	if( data == nullptr ) {
		throw std::runtime_error("Failed to load image: '" + std::string(path) + "'");
	}

	Texture* texture = new Texture( w, h, GL_RGBA, GL_RGBA );
	texture->update( data );

	stbi_image_free(data);

	return texture;

}

Screen::Screen( int width, int height ) : Texture( width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT ) {
	// noop
}

