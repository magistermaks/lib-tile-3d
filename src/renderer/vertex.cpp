
#include "vertex.hpp"

VertexAttribute::VertexAttribute( int length, GLenum type, GLsizei size, GLboolean normalize ) : length(length), type(type), size(size), normalize(normalize) {
	// noop
}

VertexConsumer::VertexConsumer(GLenum primitive, int length) : primitive(primitive), length(length) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind VBO to VAO
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

VertexConsumer::~VertexConsumer() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void VertexConsumer::bind() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VertexConsumerProvider::apply() {
	int offset = 0, index = 0;

	for( VertexAttribute attribute : this->attributes ) {

		GLHelper::vertexAttribute(
			index, 
			attribute.length,
			attribute.type,
			this->length,
			offset,
			attribute.size,
			attribute.normalize
		);

		offset += attribute.length;
		index += 1;

	}

}

void VertexConsumerProvider::setPrimitive( GLenum primitive ) {
	this->primitive = primitive;
}

void VertexConsumerProvider::attribute( int size ) {
	this->attribute( VertexAttribute( size, GL_FLOAT, sizeof(float) ) );
}

void VertexConsumerProvider::attribute( VertexAttribute attr ) {
	this->attributes.push_back(attr);
	this->length += attr.length;
}

VertexConsumer VertexConsumerProvider::get() {
	VertexConsumer consumer(this->primitive, this->length);

	// add attributes to the consumer
	this->apply();

	return consumer;
}

