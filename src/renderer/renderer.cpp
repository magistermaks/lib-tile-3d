
#include "renderer.hpp"

Layer::Layer( float z ) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenTextures(1, &tex);  

	this->genBuffer( z );

	// set texture settings
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Layer::~Layer() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &tex);
}

void Layer::genBuffer( float z ) {

	float mesh[] = {
		-1, -1,  z,  0,  0,
		 1, -1,  z,  1,  0,
		 1,  1,  z,  1,  1,
		-1,  1,  z,  0,  1
	};

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), &mesh, GL_STATIC_DRAW);

	GLHelper::vertexAttribute(0, 3, GL_FLOAT, 5, 0, sizeof(float));	
	GLHelper::vertexAttribute(1, 2, GL_FLOAT, 5, 3, sizeof(float));

}

void Layer::update( byte* buffer, int width, int height ) {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex );

	if( this->fresh ) {
		this->fresh = false;
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	}else{
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width - 1, height - 1, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	}
}

void Layer::render() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->tex);
	glBindVertexArray(this->vao);
	glDrawArrays(GL_QUADS, 0, 4);
}

byte* Layer::allocate( int width, int height ) {
	return new byte[3 * width * height];
}




Layer& Renderer::addLayer( float z ) {
	layers.emplace_back( z );
	return layers[ layers.size() - 1 ];
}

void Renderer::render() {
	for( auto& layer : layers ) {
		layer.render();
	}
}


