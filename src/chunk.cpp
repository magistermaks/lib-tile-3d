
#include "chunk.hpp"

Chunk::Chunk( byte* data, Region* region, int x, int y, int z ) {
	this->data = data;
	this->region = region;

	this->cx = x;
	this->cy = y;
	this->cz = z;

	glGenVertexArrays(1, &(this->vao));
	glGenBuffers(1, &(this->vbo));

	this->size = 0;
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &(this->vbo));
	glDeleteVertexArrays(1, &(this->vao));

	delete[] this->data;
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

					// region voxel requests are slower than local access
					// so we do it only when necessary (the voxel is at the chunk border)
					if( x == 0 || y == 0 || z == 0 || x == m || y == m || z == m ) {
						flags |= (this->region->tile(cx, cy, cz, x - 1, y, z)[3] != 255) ? 0b100000 : 0; 
						flags |= (this->region->tile(cx, cy, cz, x + 1, y, z)[3] != 255) ? 0b010000 : 0; 
						flags |= (this->region->tile(cx, cy, cz, x, y - 1, z)[3] != 255) ? 0b001000 : 0; 
						flags |= (this->region->tile(cx, cy, cz, x, y + 1, z)[3] != 255) ? 0b000100 : 0; 
						flags |= (this->region->tile(cx, cy, cz, x, y, z - 1)[3] != 255) ? 0b000010 : 0; 
						flags |= (this->region->tile(cx, cy, cz, x, y, z + 1)[3] != 255) ? 0b000001 : 0; 
					}else{
						flags |= (this->xyz(x - 1, y, z)[3] != 255) ? 0b100000 : 0; // -X
						flags |= (this->xyz(x + 1, y, z)[3] != 255) ? 0b010000 : 0; // +X
						flags |= (this->xyz(x, y - 1, z)[3] != 255) ? 0b001000 : 0; // -Y
						flags |= (this->xyz(x, y + 1, z)[3] != 255) ? 0b000100 : 0; // +Y
						flags |= (this->xyz(x, y, z - 1)[3] != 255) ? 0b000010 : 0; // -Z
						flags |= (this->xyz(x, y, z + 1)[3] != 255) ? 0b000001 : 0; // +Z
					}

					if( flags ) Mesh::buildVoxel( vertex_buffer, vox, x * 2, y * 2, z * 2, flags );

				}

			}
		}
	}

	vertex_buffer.shrink_to_fit();
	return vertex_buffer;

}

inline byte* Chunk::xyz(byte x, byte y, byte z) {
	return &(this->data[x * 0x4000 + y * 0x100 + z * 0x4]);
}

void Chunk::update() {
	auto mesh = this->build();
	this->size = mesh.size();

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->size, mesh.data(), GL_STATIC_DRAW);

	GLHelper::vertexAttribute(0, 3, GL_BYTE, 6, 0, sizeof(GLbyte), GL_FALSE);
	GLHelper::vertexAttribute(1, 3, GL_UNSIGNED_BYTE, 6, 3, sizeof(GLbyte), GL_TRUE);

	#if LT3D_PRIMITIVE == GL_QUADS
	std::string count = std::to_string(this->size / 6 / 4) + " quads"; 
	#else
	std::string count = std::to_string(this->size / 6 / 3) + " triangles"; 
	#endif

	logger::info( "Generated chunk mesh, used vertex memory: " + std::to_string(this->size) + " bytes (" + count + ")");
}

void Chunk::render( GLuint uniform ) {
		
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(this->cx * 4, this->cy * 4, this->cz * 4));
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(this->vao);
	glDrawArrays(LT3D_PRIMITIVE, 0, this->size / 6);

}

byte* Chunk::allocate() {
	double (*chunk)[64][64][4];
	chunk = new double[64][64][64][4];

	return (byte*) chunk;
}

//FIXME: UGLY WORLDGEN

byte* get(byte* arr, byte x, byte y, byte z) {
	return &(arr[x * 0x4000 + y * 0x100 + z * 0x4]);
}

void Chunk::genCube( byte* arr, byte air ) {

	const int c = 32;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				// random RGB
				for( int c = 0; c < 3; c ++ ) {
					get(arr, x, y, z)[c] = (byte) rand();
				}

				// currently alpha only supports on/off 
				get(arr, x, y, z)[3] = ( (byte) rand() ) < air ? 0 : 255;
			}
		}
	}

}

void Chunk::genBall( byte* arr, byte air ) {

	const int c = 32;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				// random RGB
				for( int c = 0; c < 3; c ++ ) {
					get(arr, x, y, z)[c] = (byte) rand();
				}

				float A = c - x, B = c - y, C = c - z;

				if( sqrt( A*A + B*B + C*C ) < c ) {
					get(arr, x, y, z)[3] = ( (byte) rand() ) < air ? 0 : 255;
				}else{
					get(arr, x, y, z)[3] = 0;
				}

			}
		}
	}
}	
