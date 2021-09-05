#pragma once

#include <config.hpp>

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

		ReusableBuffer<float> buffer;
		bool modified;

	public:

		// primitive info
		const GLenum primitive;
		const int length;

		VertexConsumer( GLenum primitive, int length );
		VertexConsumer( VertexConsumer&& consumer );
		~VertexConsumer();

		template< class... Args, class = trait::are_types_equal< float, Args... > >
		void vertex( Args... args ) {
			this->modified = true;
			this->buffer.push( args... );
		}

		void clear() {
			this->modified = true;
			this->buffer.clear();
		}

		// prepare and bind OpenGL buffers
		void bind();
		long count();

};

class VertexConsumerProvider {

	private:

		GLenum primitive = GL_TRIANGLES;
		int length = 0;
		std::vector<VertexAttribute> attributes;

		void apply();

	public:

		void setPrimitive( GLenum primitive );
		void attribute( int size );
		void attribute( VertexAttribute attr );

		VertexConsumer get();
	
};

