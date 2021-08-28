
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

	this->tex = new Texture(w, h, GL_RGBA, GL_RGBA);
	this->tex->update(data);
  
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
	delete this->tex;
}

Glyph& Charset::get( char index ) {
	return characters.at((int) index);
}

Texture& Charset::texture() {
	return *this->tex;
}

