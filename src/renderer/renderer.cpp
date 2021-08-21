
#include "renderer.hpp"

RenderSystem::RenderSystem() : vertices(16) {
	this->tex = 0;
	this->shader = nullptr;
	this->consumer = nullptr;
}

RenderSystem::~RenderSystem() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void RenderSystem::setTexture( Texture& texture ) {
	this->tex = texture.id();
}

void RenderSystem::setTexture( GLuint texture ) {
	this->tex = texture;
}

void RenderSystem::setShader( ShaderProgram& shader ) {
	this->shader = &shader;
}

void RenderSystem::setConsumer( VertexConsumer& consumer ) {
	this->consumer = &consumer;
}

void RenderSystem::vertex2f( float x, float y, float u, float v ) {
	vertices.push(x, y, u, v);
}

void RenderSystem::vertex3f( float x, float y, float z, float u, float v ) {
	vertices.push(x, y, z, u, v);
}

void RenderSystem::drawText( const std::string& text, float x, float y, float size, Charset& charset ) {

	this->setTexture( charset.texture() );

	for( char letter : text ) {
		Glyph& glyph = charset.get(letter);

		const float w = size;
		const float h = size;

		this->vertex2f( x, y, glyph.uv.x, glyph.uv.y );
		this->vertex2f( x + w, y, glyph.uv.x + glyph.size.x, glyph.uv.y );
		this->vertex2f( x, y + h, glyph.uv.x, glyph.uv.y + glyph.size.y );

		this->vertex2f( x + w, y, glyph.uv.x + glyph.size.x, glyph.uv.y );
		this->vertex2f( x + w, y + h, glyph.uv.x + glyph.size.x, glyph.uv.y + glyph.size.y );
		this->vertex2f( x, y + h, glyph.uv.x, glyph.uv.y + glyph.size.y );

		x += w;
	}

	this->draw();
}

void RenderSystem::drawScreen( Screen& screen ) {
	this->setTexture( screen );

	this->vertex2f( -1, -1,  0,  0 );
	this->vertex2f(  1, -1,  1,  0 );
	this->vertex2f( -1,  1,  0,  1 );

	this->vertex2f(  1,  1,  1,  1 );
	this->vertex2f( -1,  1,  0,  1 );
	this->vertex2f(  1, -1,  1,  0 );

	this->draw();
}

void RenderSystem::draw() {
	if( !vertices.empty() ) {

		assert( this->shader != nullptr );

		// bind given texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->tex);

		// bind vertex buffer
		this->consumer->bind();		

		// update buffer
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);

		// bind shader
		this->shader->bind();

		// call OpenGL
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3 );
		vertices.clear();

	}
}

void RenderSystem::flush() {

	// glFlush() can also be used on SOME drivers,
	// not flushing at all can work but depends on undefined behaviour.
	glFinish();

}

RenderSystem& RenderSystem::instance() {
	static RenderSystem renderer;
	return renderer;
}

