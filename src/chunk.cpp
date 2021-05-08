
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

	Chunk* l = nullptr;
	Chunk* r = nullptr;
	Chunk* d = nullptr;
	Chunk* u = nullptr;
	Chunk* f = nullptr;
	Chunk* b = nullptr;
	
	try{ l = region->chunk(cx - 1, cy, cz); }catch(...){}
	try{ r = region->chunk(cx + 1, cy, cz); }catch(...){}
	try{ d = region->chunk(cx, cy - 1, cz); }catch(...){}
	try{ u = region->chunk(cx, cy + 1, cz); }catch(...){}
	try{ f = region->chunk(cx, cy, cz - 1); }catch(...){}
	try{ b = region->chunk(cx, cy, cz + 1); }catch(...){}

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				const byte* vox = this->xyz(x, y, z);

				if( vox[3] != 0 ) {

					byte flags = 0;

					if( x == 0 ) {
						flags |= (l == nullptr || l->xyz(63, y, z)[3] != 255) * 0b100000;
					} else {
						flags |= (this->xyz(x - 1, y, z)[3] != 255) * 0b100000;

						if( x == 63 ) {
							flags |= (r == nullptr || r->xyz(0, y, z)[3] != 255) * 0b010000;
						} else {
							flags |= (this->xyz(x + 1, y, z)[3] != 255) * 0b010000;
						}
					}

					if( y == 0 ) {
						flags |= (d == nullptr || d->xyz(x, 63, z)[3] != 255) * 0b001000;
					} else {
						flags |= (this->xyz(x, y - 1, z)[3] != 255) * 0b001000;

						if( y == 63 ) {
							flags |= (u == nullptr || u->xyz(x, 0, z)[3] != 255) * 0b000100;
						} else {
							flags |= (this->xyz(x, y + 1, z)[3] != 255) * 0b000100;
						}
					}

					if( z == 0 ) {
						flags |= (f == nullptr || f->xyz(x, y, 63)[3] != 255) * 0b000010;
					} else {
						flags |= (this->xyz(x, y, z - 1)[3] != 255) * 0b000010;

						if( z == 63 ) {
							flags |= (b == nullptr || b->xyz(x, y, 0)[3] != 255) * 0b000001;
						} else {
							flags |= (this->xyz(x, y, z + 1)[3] != 255) * 0b000001;
						}
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

	Clock::time_point point = Clock::now();

	auto mesh = this->build();
	this->size = mesh.size();

	if( this->size != 0 ) {

		glBindVertexArray(this->vao);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, this->size, mesh.data(), GL_STATIC_DRAW);

		GLHelper::vertexAttribute(0, 3, GL_BYTE, 6, 0, sizeof(GLbyte), GL_FALSE);
		GLHelper::vertexAttribute(1, 3, GL_UNSIGNED_BYTE, 6, 3, sizeof(GLbyte), GL_TRUE);

	}

	#if LT3D_PRIMITIVE == GL_QUADS
	std::string count = std::to_string(this->size / 6 / 4) + " quads"; 
	#else
	std::string count = std::to_string(this->size / 6 / 3) + " triangles"; 
	#endif

	long ms = std::chrono::duration_cast<milliseconds>( Clock::now() - point ).count();

	logger::info( "Generated chunk mesh, used vertex memory: " + std::to_string(this->size) + " bytes (" + count + ") took: " + std::to_string(ms) + "ms");
}

void Chunk::render( GLuint uniform ) {

	if( this->size != 0 ) {
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(this->cx * 4, this->cy * 4, this->cz * 4));
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->vao);
		glDrawArrays(LT3D_PRIMITIVE, 0, this->size / 6);

	}

}

byte* Chunk::allocate() {
	byte (*chunk)[64][64][4];
	chunk = new byte[64][64][64][4];

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

void Chunk::genBall( byte* arr, byte air, int r ) {

	const int c = 32;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				// random RGB
				for( int h = 0; h < 3; h ++ ) {
					get(arr, x, y, z)[h] = (byte) rand();
				}

				float A = c - x, B = c - y, C = c - z;

				if( sqrt( A*A + B*B + C*C ) < r ) {
					get(arr, x, y, z)[3] = ( (byte) rand() ) < air ? 0 : 255;
				}else{
					get(arr, x, y, z)[3] = 0;
				}

			}
		}
	}
}	
