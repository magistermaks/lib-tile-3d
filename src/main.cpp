
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "logger.hpp"
#include "glhelper.hpp"
#include "mesh.hpp"

#ifdef WIN32 
#	include <windows.h>
#	include <direct.h>
#	define CWD_MAX_PATH MAX_PATH
#	define POSIX_GETCWD _getcwd
#else
#	include <unistd.h>
#	include <sys/stat.h>
#	define CWD_MAX_PATH PATH_MAX
#	define POSIX_GETCWD getcwd
#endif

int main( void ) {

	const int width = 1024;
	const int height = 768;
	const int size = 32;

	// print cwd, nice for debugging
	{  
		char temp[ CWD_MAX_PATH ];
		logger::info( "Current working directory: '" + std::string( POSIX_GETCWD(temp, sizeof(temp)) ? temp : "" ) + "'" );
	}

	// initilize GLFW and GLEW
	if( !GLHelper::init(width, height, "LibTile3D") ) {
		return -1;
	}
	
	GLFWwindow* window = GLHelper::window();

	byte arr[size][size][size][3]; // x => y => z => [r, g, b]

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {
				for( int c = 0; c < 3; c ++ ) {
					arr[x][y][z][c] = (byte) rand();
				}
			}
		}
	}

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders();

	auto vertex_buffer_data = Mesh::build( (byte*) arr, size );
	auto vertex_buffer_size = vertex_buffer_data.size() * sizeof(GLfloat);
	
	logger::info( std::string("Cube size: ") + std::to_string(size) + ", used vertex memory: " + std::to_string(vertex_buffer_size) + " bytes");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_data.data(), GL_STATIC_DRAW);

	//GLHelper::vertexAttribute(0, 3, GL_FLOAT, 6, 0, sizeof(GLfloat));
	//GLHelper::vertexAttribute(1, 4, GL_BYTE, 6, 3, sizeof(GLfloat), true);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*) (long) 0);
	glEnableVertexAttribArray(0);

	// get RGBA from a single 4-byte packed float, and normalize it into a 0-1 float
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLfloat), (GLvoid*) (long) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// this data is now copied to the GPU, we don't need it here anymore
	vertex_buffer_data.clear();

	// get locations from sahder program
	GLuint modelLoc = program.location("model");
	GLuint viewLoc = program.location("view");
	GLuint projectionLoc = program.location("projection");
 
	do {

		// fancy location
		float camX = sin(glfwGetTime()) * 10;
		float camZ = cos(glfwGetTime()) * 10;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, camZ, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); 

		// clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// enable shader program
		program.bind();

		// pass matricies to GPU
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, size * size * size * 6 * 2 * 3);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while( glfwWindowShouldClose(window) == 0 );

	// free VBOs
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &vertex_array);

	// close window
	glfwTerminate();

	return 0;
}


