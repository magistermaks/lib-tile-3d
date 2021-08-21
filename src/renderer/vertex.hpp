#pragma once

#include "../config.hpp"

struct VertexAttribute {

	const int length;
	const GLenum type;
	const GLsizei size;
	const GLboolean normalize;

	VertexAttribute( int length, GLenum type, GLsizei size, GLboolean normalize = GL_FALSE );

};

class VertexConsumer {

	private:

		GLuint vbo, vao;

	public:

		VertexConsumer();
		~VertexConsumer();

		void bind();

};

class VertexConsumerProvider {

	private:

		int length = 0;
		std::vector<VertexAttribute> attributes;

		void apply();

	public:

		void attribute( int size );
		void attribute( VertexAttribute attr );

		VertexConsumer get();
	
};

