
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

std::vector<byte> Chunk::build() {
	
	std::vector<byte> vertex_buffer;

	const byte m = 63;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				byte* vox = this->xyz(x, y, z);
				if( vox[3] != 0 ) {

					byte flags = 0;
					flags |= (x == 0 || this->xyz(x - 1, y, z)[3] != 255) ? 0b100000 : 0; // -X
					flags |= (x == m || this->xyz(x + 1, y, z)[3] != 255) ? 0b010000 : 0; // +X
					flags |= (y == 0 || this->xyz(x, y - 1, z)[3] != 255) ? 0b001000 : 0; // -Y
					flags |= (y == m || this->xyz(x, y + 1, z)[3] != 255) ? 0b000100 : 0; // +Y
					flags |= (z == 0 || this->xyz(x, y, z - 1)[3] != 255) ? 0b000010 : 0; // -Z
					flags |= (z == m || this->xyz(x, y, z + 1)[3] != 255) ? 0b000001 : 0; // +Z

					if( flags ) Mesh::buildVoxel( vertex_buffer, vox, x * 2, y * 2, z * 2, flags );

				}

			}
		}
	}

	vertex_buffer.shrink_to_fit();
	return vertex_buffer;

}

byte* Chunk::xyz(byte x, byte y, byte z) {
	return &(this->data[x * 0x4000 + y * 0x100 + z * 0x4]);
}

void Chunk::update() {
	auto mesh = this->build();
	this->size = mesh.size();

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->size, mesh.data(), GL_STATIC_DRAW);

	GLHelper::vertexAttribute(0, 3, GL_BYTE, 7, 0, sizeof(GLbyte), GL_FALSE);
	GLHelper::vertexAttribute(1, 4, GL_UNSIGNED_BYTE, 7, 3, sizeof(GLbyte), GL_TRUE);

	#if LT3D_PRIMITIVE == GL_QUADS
	std::string count = std::to_string(this->size / 7 / 4) + " quads"; 
	#else
	std::string count = std::to_string(this->size / 7 / 3) + " triangles"; 
	#endif

	logger::info( "Generated chunk mesh, used vertex memory: " + std::to_string(this->size) + " bytes (" + count + ")");
}

void Chunk::render( float x, float y, float z, GLuint uniform ) {
		
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(this->vao);
	glDrawArrays(LT3D_PRIMITIVE, 0, this->size / 7);

}

void Chunk::genCube( chunk_t arr, byte air ) {

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

void Chunk::genBall( chunk_t arr, byte air ) {

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
