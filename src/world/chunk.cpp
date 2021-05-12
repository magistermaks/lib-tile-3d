
#include "chunk.hpp"

Chunk::Chunk( Voxel* data, Region* region, int x, int y, int z ) {
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

Mesh::StaticBuffer* Chunk::build( void* ptr ) {

	Clock::time_point point = Clock::now();

	Mesh::ReusableBuffer* buffer = (Mesh::ReusableBuffer*) ptr;

	Chunk* l = region->chunk(cx - 1, cy, cz);
	Chunk* r = region->chunk(cx + 1, cy, cz);
	Chunk* d = region->chunk(cx, cy - 1, cz);
	Chunk* u = region->chunk(cx, cy + 1, cz);
	Chunk* f = region->chunk(cx, cy, cz - 1);
	Chunk* b = region->chunk(cx, cy, cz + 1);

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				const Voxel* vox = this->xyz(x, y, z);

				if( vox->a != 0 ) {

					byte flags = 0;

					if( x == 0 ) {
						flags |= (l == nullptr || l->xyz(63, y, z)->a != 255) * 0b100000;
					} else {
						flags |= (this->xyz(x - 1, y, z)->a != 255) * 0b100000;

						if( x == 63 ) {
							flags |= (r == nullptr || r->xyz(0, y, z)->a != 255) * 0b010000;
						} else {
							flags |= (this->xyz(x + 1, y, z)->a != 255) * 0b010000;
						}
					}

					if( y == 0 ) {
						flags |= (d == nullptr || d->xyz(x, 63, z)->a != 255) * 0b001000;
					} else {
						flags |= (this->xyz(x, y - 1, z)->a != 255) * 0b001000;

						if( y == 63 ) {
							flags |= (u == nullptr || u->xyz(x, 0, z)->a != 255) * 0b000100;
						} else {
							flags |= (this->xyz(x, y + 1, z)->a != 255) * 0b000100;
						}
					}

					if( z == 0 ) {
						flags |= (f == nullptr || f->xyz(x, y, 63)->a != 255) * 0b000010;
					} else {
						flags |= (this->xyz(x, y, z - 1)->a != 255) * 0b000010;

						if( z == 63 ) {
							flags |= (b == nullptr || b->xyz(x, y, 0)->a != 255) * 0b000001;
						} else {
							flags |= (this->xyz(x, y, z + 1)->a != 255) * 0b000001;
						}
					}
					
					if( flags ) Mesh::buildVoxel( *buffer, vox, x * 2, y * 2, z * 2, flags );

				}

			}
		}
	}

	size_t size = buffer->size();

	#if LT3D_PRIMITIVE == GL_QUADS
	std::string count = std::to_string(size / 6 / 4) + " quads"; 
	#else
	std::string count = std::to_string(size / 6 / 3) + " triangles"; 
	#endif

	long ms = std::chrono::duration_cast<milliseconds>( Clock::now() - point ).count();

	logger::info( "Generated chunk mesh, used vertex memory: " + std::to_string(size) + " bytes (" + count + ") took: " + std::to_string(ms) + "ms");

	return new Mesh::StaticBuffer(buffer);

}

inline Voxel* Chunk::xyz(byte x, byte y, byte z) {
	return &(this->data[x * 4096 + y * 64 + z]);
}

void Chunk::update( Mesh::StaticBuffer* mesh ) {

	this->size = mesh->size;

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->size, mesh->data, GL_STATIC_DRAW);

	GLHelper::vertexAttribute(0, 3, GL_BYTE, 6, 0, sizeof(GLbyte), GL_FALSE);
	GLHelper::vertexAttribute(1, 3, GL_UNSIGNED_BYTE, 6, 3, sizeof(GLbyte), GL_TRUE);

	delete mesh;

}

void Chunk::render( GLuint uniform ) {

	if( this->size != 0 ) {
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(this->cx * 4, this->cy * 4, this->cz * 4));
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->vao);
		glDrawArrays(LT3D_PRIMITIVE, 0, this->size / 6);

	}

}

void Chunk::discard() {
	this->size = 0;
}

Voxel* Chunk::allocate() {
	Voxel (*chunk)[64][64];
	chunk = new Voxel[64][64][64];

	return (Voxel*) chunk;
}

//FIXME: UGLY WORLDGEN, MOVE ELSEWHERE

Voxel& get(Voxel* arr, byte x, byte y, byte z) {
	return arr[x * 4096 + y * 64 + z];
}

void Chunk::genBall( Voxel* arr, byte air, int r ) {

	const int c = 32;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {

				// random RGB
				get(arr, x, y, z) = Voxel::random();

				float A = c - x, B = c - y, C = c - z;

				if( sqrt( A*A + B*B + C*C ) < r ) {
					get(arr, x, y, z).a = ( (byte) rand() ) < air ? 0 : 255;
				}else{
					get(arr, x, y, z).a = 0;
				}

			}
		}
	}
}	
