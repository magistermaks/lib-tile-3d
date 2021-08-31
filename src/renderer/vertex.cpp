
#include "vertex.hpp"

VertexAttribute::VertexAttribute( int length, GLenum type, GLsizei size, GLboolean normalize ) : length(length), type(type), size(size), normalize(normalize) {
	// noop
}

VertexConsumer::VertexConsumer(GLenum primitive, int length) : primitive(primitive), length(length), buffer(16) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind VBO to VAO
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	modified = false;
}

VertexConsumer::VertexConsumer( VertexConsumer&& consumer ) : vao(consumer.vao), vbo(consumer.vbo), primitive(consumer.primitive), length(consumer.length), buffer(std::move(consumer.buffer)) {
	consumer.vao = 0;
	consumer.vbo = 0;

	modified = consumer.modified;
}

VertexConsumer::~VertexConsumer() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void VertexConsumer::bind() {
	glBindVertexArray(vao);

	// update VBO if something was written to vertex buffer
	if( this->modified ) {
		this->modified = false;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, this->buffer.size() * sizeof(float), this->buffer.data(), GL_DYNAMIC_DRAW);
	}
}

long VertexConsumer::count() {
	return this->buffer.size() / this->length;
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

	return std::move(consumer);
}

