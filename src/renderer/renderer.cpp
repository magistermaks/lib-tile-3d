
#include "renderer.hpp"

Canvas::Canvas( int width, int height ) {
	this->width = width;
	this->height = height;
	this->fresh = true;

	glGenTextures(1, &tex);  

	// set texture settings
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Canvas::~Canvas() {
	glDeleteTextures(1, &tex);
}

void Canvas::update( byte* buffer ) {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex );

	/* C++20: [[unlikely]] */ 
	if( this->fresh ) {
		this->fresh = false;
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	}else{
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	}

}

GLuint Canvas::id() {
	return this->tex;
}

byte* Canvas::allocate( int width, int height ) {
	return new byte[3 * width * height];
}

RenderSystem::RenderSystem() : vertices(16) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind VBO to VAO
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	// set buffer attributes
	GLHelper::vertexAttribute(0, 2, GL_FLOAT, 4, 0, sizeof(float));
	GLHelper::vertexAttribute(1, 2, GL_FLOAT, 4, 2, sizeof(float));

	this->tex = 0;
	this->shader = nullptr;
}

RenderSystem::~RenderSystem() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void RenderSystem::setTexture( GLuint tex ) {
	this->tex = tex;
}

void RenderSystem::setShader( GLHelper::ShaderProgram& shader ) {
	this->shader = &shader;
}

void RenderSystem::vertex( float x, float y, float u, float v ) {
	vertices.push(x, y, u, v);
}

void RenderSystem::drawText( const std::string& text, float x, float y, float size, Charset& charset ) {

	this->setTexture( charset.texture() );

	for( char letter : text ) {
		Glyph& glyph = charset.get(letter);

		const float w = size;
		const float h = size;

		this->vertex( x, y, glyph.uv.x, glyph.uv.y );
		this->vertex( x + w, y, glyph.uv.x + glyph.size.x, glyph.uv.y );
		this->vertex( x + w, y + h, glyph.uv.x + glyph.size.x, glyph.uv.y + glyph.size.y );
		this->vertex( x, y + h, glyph.uv.x, glyph.uv.y + glyph.size.y );

		x += w;
	}

	this->draw();
}

void RenderSystem::drawScreen( Canvas& canvas ) {
	this->setTexture( canvas.id() );
	this->vertex( -1, -1,  0,  0 );
	this->vertex(  1, -1,  1,  0 );
	this->vertex(  1,  1,  1,  1 );
	this->vertex( -1,  1,  0,  1 );
	this->draw();

}

void RenderSystem::draw() {
	if( !vertices.empty() ) {

		assert( this->shader != nullptr );

		// bind given texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->tex);

		// bind VAO and VBO
		glBindVertexArray(this->vao);

		// update buffer
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);

		// bind shader
		this->shader->bind();

		// call OpenGL
		glDrawArrays(GL_QUADS, 0, vertices.size() / 4 );
		vertices.clear();

	}

}

RenderSystem& RenderSystem::instance() {
	static RenderSystem renderer;
	return renderer;
}

