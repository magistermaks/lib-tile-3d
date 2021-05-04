
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
	if( !GLHelper::init(width, height, "LibTile3D | FPS: 0") ) {
		return -1;
	}
	
	GLFWwindow* window = GLHelper::window();

	byte arr[size][size][size][4]; // x => y => z => [r, g, b, a]

	for( int x = 0; x < size; x ++ ) {
		for( int y = 0; y < size; y ++ ) {
			for( int z = 0; z < size; z ++ ) {
				for( int c = 0; c < 3; c ++ ) {
					arr[x][y][z][c] = (byte) rand();
				}

				// currently alpha only supports on/off 
				arr[x][y][z][3] = ( (byte) rand() ) < 100 ? 0 : 255;
			}
		}
	}

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders();

	auto vertex_buffer_data = Mesh::build( (byte*) arr, size );
	auto vertex_buffer_size = vertex_buffer_data.size(); //* sizeof(GLbyte);
	
	logger::info( std::string("Cube size: ") + std::to_string(size) + ", used vertex memory: " + std::to_string(vertex_buffer_size) + " bytes");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_data.data(), GL_STATIC_DRAW);
	
	// 0 => xyz as floats
	// 1 => get RGBA from a single 4-byte packed float, and normalize it into a 0-1 float
	GLHelper::vertexAttribute(0, 3, GL_BYTE, 7, 0, sizeof(GLbyte), GL_FALSE);
	GLHelper::vertexAttribute(1, 4, GL_UNSIGNED_BYTE, 7, 3, sizeof(GLbyte), GL_TRUE);

	// this data is now copied to the GPU, we don't need it here anymore
	vertex_buffer_data.clear();

	// get locations from sahder program
	GLuint modelLoc = program.location("model");
	GLuint viewLoc = program.location("view");
	GLuint projectionLoc = program.location("projection");
	GLuint sizeLoc = program.location("size");

	time_t last = 0;
	long count = 0;

	// remove frame cap - for performance testing
	// it's not 100% relible on all systems/drivers
	glfwSwapInterval(0);
 
	do {

		if( last != time(0) ) {
			std::string title = "LibTile3D | FPS: " + std::to_string(count);
			glfwSetWindowTitle(window, title.c_str());
			last = time(0);
			count = 0;
		}

		// fancy location
		float camX = sin( glfwGetTime() ) * 10;
		float camZ = cos( glfwGetTime() ) * 10;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, camZ, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); 
		view = glm::translate( view, glm::vec3(-1, -1, -1));

		// clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// enable shader program
		program.bind();

		// pass matricies to GPU
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniform1f(sizeLoc, ((float) size / 2));

		// bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_size / 7);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		count ++;

	} while( glfwWindowShouldClose(window) == 0 );

	// free VBOs
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &vertex_array);

	// close window
	glfwTerminate();

	return 0;
}


