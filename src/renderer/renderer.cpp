
#include "renderer.hpp"

RenderSystem::RenderSystem() {
	this->shader = nullptr;
	this->consumer = nullptr;
	this->texture = nullptr;
}

void RenderSystem::setTexture( Texture& texture ) {
	this->texture = &texture;
}

void RenderSystem::setShader( ShaderProgram& shader ) {
	this->shader = &shader;
}

void RenderSystem::setConsumer( VertexConsumer& consumer ) {
	this->consumer = &consumer;
}

void RenderSystem::setDepthFunc( GLenum depth ) {
	this->depth = depth;
}

void RenderSystem::vertex( float x, float y ) {
	this->consumer->vertex(x, y);
}

void RenderSystem::vertex( float x, float y, float z ) {
	this->consumer->vertex(x, y, z);
}

void RenderSystem::vertex( float x, float y, float u, float v ) {
	this->consumer->vertex(x, y, u, v);
}

void RenderSystem::vertex( float x, float y, float z, float u, float v ) {
	this->consumer->vertex(x, y, z, u, v);
}

void RenderSystem::clear() {
	this->consumer->clear();
}

void RenderSystem::depthTest( bool flag ) {
	if( flag ) glDepthFunc(this->depth); else glDepthFunc(GL_ALWAYS);
}

void RenderSystem::depthMask( bool flag ) {
	glDepthMask(flag);
}

void RenderSystem::drawText( const std::string& text, float x, float y, float size, Charset& charset ) {

	this->setTexture( charset.texture() );

	for( char letter : text ) {
		Glyph& glyph = charset.get(letter);

		const float w = size;
		const float h = size;

		this->vertex( x, y, glyph.uv.x, glyph.uv.y );
		this->vertex( x + w, y, glyph.uv.x + glyph.size.x, glyph.uv.y );
		this->vertex( x, y + h, glyph.uv.x, glyph.uv.y + glyph.size.y );

		this->vertex( x + w, y, glyph.uv.x + glyph.size.x, glyph.uv.y );
		this->vertex( x + w, y + h, glyph.uv.x + glyph.size.x, glyph.uv.y + glyph.size.y );
		this->vertex( x, y + h, glyph.uv.x, glyph.uv.y + glyph.size.y );

		x += w;
	}

	this->draw();
	this->clear();
}

void RenderSystem::drawScreen( Screen& screen ) {
	this->setTexture( screen );

	this->vertex( -1, -1,  0,  0 );
	this->vertex(  1, -1,  1,  0 );
	this->vertex( -1,  1,  0,  1 );

	this->vertex(  1,  1,  1,  1 );
	this->vertex( -1,  1,  0,  1 );
	this->vertex(  1, -1,  1,  0 );

	this->draw();
	this->clear();
}

void RenderSystem::draw() {

	assert( this->shader != nullptr );
	assert( this->consumer != nullptr );

	// bind given texture
	this->texture->bind();

	// bind vertex buffer
	this->consumer->bind();

	// bind shader
	this->shader->bind();

	// call OpenGL
	glDrawArrays(this->consumer->primitive, 0, this->consumer->count());
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

