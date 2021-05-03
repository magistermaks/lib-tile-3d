
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "logger.hpp"
#include "shader.hpp"

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

typedef unsigned char byte;

#define S 32
#define width 1024
#define height 768

void putIndice( std::vector<GLfloat>& vec, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b ) {
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	vec.push_back(r);
	vec.push_back(g);
	vec.push_back(b);
}

void putQuad( std::vector<GLfloat>& vec, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4, GLfloat r, GLfloat g, GLfloat b ) {
	putIndice( vec, x1, y1, z1, r, g, b );
	putIndice( vec, x2, y2, z2, r, g, b );
	putIndice( vec, x3, y3, z3, r, g, b );
    putIndice( vec, x2, y2, z2, r, g, b );
	putIndice( vec, x4, y4, z4, r, g, b );
	putIndice( vec, x3, y3, z3, r, g, b );
}

void putVoxel( std::vector<GLfloat>& vec, byte R, byte G, byte B, int X, int Y, int Z ) {

	const float s = 1.f / S;
	const float x = ((float) X / (float) S) * 2 - 1.f + s;
	const float y = ((float) Y / (float) S) * 2 - 1.f + s;
	const float z = ((float) Z / (float) S) * 2 - 1.f + s;
	const float r = R / 255.0f;
	const float g = G / 255.0f;
	const float b = B / 255.0f;

	// 3---->4   3<----4    3-----4
	//   \       | \   |    |     |
	//    \   => |  \  | => |     |
	//     \     |   \ |    |     |
	// 1---->2   1---->2    1-----2
    //
	//                          x   y   z               x   y   z
	//    e-------f      a = ( -1,  1, -1 )  =>  e = ( -1,  1,  1 )
	//   /|      /|      b = (  1,  1, -1 )  =>  f = (  1,  1,  1 )
	//  / |     / |      c = ( -1, -1, -1 )  =>  g = ( -1, -1,  1 )
	// a--|----b  |  =>  d = (  1, -1, -1 )  =>  h = (  1, -1,  1 )
	// |  g----|--h    
	// | /     | /       
	// c-------d 
	//
	// a: ( x - s, y + s, z - s )
	// b: ( x + s, y + s, z - s )
	// c: ( x - s, y - s, z - s )
	// d: ( x + s, y - s, z - s )
	// e: ( x - s, y + s, z + s )
	// f: ( x + s, y + s, z + s )
	// g: ( x - s, y - s, z + s )
	// h: ( x + s, y - s, z + s )
	//
	//            1      1      1      2      2      2      3      3      3      4      4      4      R  G  B
	putQuad( vec, x - s, y - s, z - s, x + s, y - s, z - s, x - s, y + s, z - s, x + s, y + s, z - s, r, g, b ); // (c => d => a => b) => front 
	putQuad( vec, x + s, y - s, z + s, x - s, y - s, z + s, x + s, y + s, z + s, x - s, y + s, z + s, r, g, b ); // (h => g => f => e) => back
	putQuad( vec, x - s, y - s, z + s, x - s, y - s, z - s, x - s, y + s, z + s, x - s, y + s, z - s, r, g, b ); // (g => c => e => a) => left
	putQuad( vec, x + s, y - s, z - s, x + s, y - s, z + s, x + s, y + s, z - s, x + s, y + s, z + s, r, g, b ); // (d => h => b => f) => right
	putQuad( vec, x - s, y + s, z - s, x + s, y + s, z - s, x - s, y + s, z + s, x + s, y + s, z + s, r, g, b ); // (a => b => e => f) => top
	putQuad( vec, x - s, y - s, z + s, x + s, y - s, z + s, x - s, y - s, z - s, x + s, y - s, z - s, r, g, b ); // (g => h => c => d) => bottom
		
}

int main( void ) {

	srand ((unsigned int) time (NULL));

    {  
	    char temp[ CWD_MAX_PATH ];
	    std::string path( POSIX_GETCWD(temp, sizeof(temp)) ? temp : "" );
        logger::info( "Current working directory: " + path );
    }

    byte arr[S][S][S][3]; // x => y => z => [r, g, b]

	for( int x = 0; x < S; x ++ ) {
		for( int y = 0; y < S; y ++ ) {
			for( int z = 0; z < S; z ++ ) {
				for( int c = 0; c < 3; c ++ ) {
					arr[x][y][z][c] = (byte) rand();
				}
			}
		}
	}

	if( glfwInit() != true ) {
        logger::fatal( "Failed to initialize GLFW!" );
        return -1;
	}

//	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow( width, height, "TerraTile", NULL, NULL);

	if( window == NULL ) {
        logger::fatal( "Failed to open GLFW window!" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glewExperimental = true;

    if( glewInit() != GLEW_OK ) {
        logger::fatal( "Failed to initialize GLEW!" );
        glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "shader/vertex.glsl", "shader/fragment.glsl" );

	std::vector<GLfloat> vertex_buffer_data;

	for( int x = 0; x < S; x ++ ) {
		for( int y = 0; y < S; y ++ ) {
			for( int z = 0; z < S; z ++ ) {
				byte* rgb = (byte*) arr[x][y][z];
				putVoxel( vertex_buffer_data, rgb[0], rgb[1], rgb[2], x, y, z );
			}
		}
	}

	vertex_buffer_data.shrink_to_fit();

	std::cout << "INFO: Cube size: " << S << ", used vertex memory: " << vertex_buffer_data.size() * sizeof(GLfloat) << " bytes\n";

//	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);  
//	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
//	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
//	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
//	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
//
//	
//	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const float radius = 10.0f; 

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

	glEnable(GL_DEPTH_TEST);  

//	std::cout << "\nVBO: ";
//
//	vertex_buffer_data.shrink_to_fit();
//
//	for( GLfloat c : vertex_buffer_data ) {
//		std::cout << c << " ";
//	} 
//
//	std::cout << "\n";
//
//	static const GLfloat gvertex_buffer_data[] = { 
//		-1.0f, -1.0f, 0.0f, 1, 0, 1,
//		 1.0f, -1.0f, 0.0f,  0, 1, 1,
//		-1.0f,  1.0f, 0.0f, 1, 1, 0,
//
//       1.0f, -1.0f, 0.0f, 1, 0, 0,
//		 1.0f,  1.0f, 0.0f, 1, 1, 0,
//		-1.0f,  1.0f, 0.0f, 0, 1, 1,
//	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(gvertex_buffer_data), gvertex_buffer_data, GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data.size(), vertex_buffer_data.data(), GL_STATIC_DRAW);

	// loctaions
	unsigned int modelLoc = glGetUniformLocation(programID, "model");
	unsigned int viewLoc = glGetUniformLocation(programID, "view");
	unsigned int projectionLoc = glGetUniformLocation(programID, "projection");
 
	do{

		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, camZ, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); 

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// 1rst attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			6 * sizeof(GLfloat),  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			6 * sizeof(GLfloat),  // stride
			(void*)(3 * sizeof(GLfloat)) // array buffer offset
		);
		glEnableVertexAttribArray(1);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, S*S*S*6*2*3); // 3 indices starting at 0 -> 1 triangle
//		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


