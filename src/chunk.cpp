
#include "chunk.hpp"

Chunk::Chunk( byte* data ) {
	this->data = data;

	glGenVertexArrays(1, &(this->vao));
	glGenBuffers(1, &(this->vbo));

	this->size = 0;
	this->update();
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &(this->vbo));
	glDeleteVertexArrays(1, &(this->vao));
}

void Chunk::update() {
	auto mesh = Mesh::build( this->data, 64 );
	this->size = mesh.size();

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->size, mesh.data(), GL_STATIC_DRAW);

	GLHelper::vertexAttribute(0, 3, GL_BYTE, 7, 0, sizeof(GLbyte), GL_FALSE);
	GLHelper::vertexAttribute(1, 4, GL_UNSIGNED_BYTE, 7, 3, sizeof(GLbyte), GL_TRUE);

	logger::info( "Generated chunk mesh, used vertex memory: " + std::to_string(this->size) + " bytes (" + std::to_string(this->size / 7) + " voxels)");
}

void Chunk::render( float x, float y, float z, GLuint uniform ) {
		
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->size / 7);

}

void Chunk::genCube( byte arr[64][64][64][4], byte air ) {

	int size = 64;
	int c = size / 2;

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {

				// random RGB
				for( int c = 0; c < 3; c ++ ) {
					arr[x][y][z][c] = (byte) rand();
				}

				// currently alpha only supports on/off 
				arr[x][y][z][3] = ( (byte) rand() ) < air ? 0 : 255;
			}
		}
	}

}

void Chunk::genBall( byte arr[64][64][64][4], byte air ) {

		int size = 64;
	int c = size / 2;

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {

				// random RGB
				for( int c = 0; c < 3; c ++ ) {
					arr[x][y][z][c] = (byte) rand();
				}

				float A = c - x, B = c - y, C = c - z;

				if( sqrt( A*A + B*B + C*C ) < c ) {
					arr[x][y][z][3] = ( (byte) rand() ) < air ? 0 : 255;
				}else{
					arr[x][y][z][3] = 0;
				}

			}
		}
	}
}	
