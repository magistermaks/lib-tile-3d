
#include "charset.hpp"

Charset::Charset( const char* path, int size ) {
	
	int w, h, n;
	byte* data = stbi_load(path, &w, &h, &n, 4);

	if( data == NULL ) {
		logger::error( "Failed to load font image: '" + std::string(path) + "'" );
		throw std::runtime_error("Font error");
	}

	if( w % size != 0 || h % size != 0 ) {
		logger::error( "Invalid size of font: '" + std::string(path) + "'" );
		throw std::runtime_error("Font error");
	} 

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
	glm::fvec2 siz( size / float(w), size / float(h) );

	for( int y = 0; y < h; y += size ) {
		for( int x = 0; x < w; x += size ) {

			glm::fvec2 uv( x / float(w), (h-y-8) / float(h) );

			Glyph glyph;
			glyph.size = siz;
			glyph.uv = uv;

			characters.push_back( glyph );
		}
	}

	stbi_image_free(data);

}

Charset::~Charset() {
	glDeleteTextures(1, &tex);
}

Glyph& Charset::get( char index ) {
	return characters.at((int) index);
}

GLuint Charset::texture() {
	return this->tex;
}

