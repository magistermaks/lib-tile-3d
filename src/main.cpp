
#include "config.hpp"

int main( void ) {

	const int width = 1024;
	const int height = 768;

	// print cwd, nice for debugging
	{  
		char temp[ CWD_MAX_PATH ];
		logger::info( "Current working directory: '" + std::string( POSIX_GETCWD(temp, sizeof(temp)) ? temp : "" ) + "'" );
	}

	// initilize GLFW, GLEW, and OpenGL
	if( !GLHelper::init(width, height, "LibTile3D | FPS: 0") ) {
		return -1;
	}
	
	GLFWwindow* window = GLHelper::window();

	logger::info("Generating voxel data...");
	chunk_t arr1, arr2;
	Chunk::genCube( arr1, 0 );
	Chunk::genBall( arr2, 0 );

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders();

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	 
	logger::info("Generating vertex data...");
	Chunk chunkA( (byte*) arr1 );
	Chunk chunkB( (byte*) arr2 );
	Chunk chunkC( (byte*) arr1 );
	Chunk chunkD( (byte*) arr2 );

	// get locations from sahder program
	GLuint modelLoc = program.location("model");
	GLuint viewLoc = program.location("view");
	GLuint projectionLoc = program.location("projection");

	time_t last = 0;
	long count = 0;

	// remove frame cap - for performance testing
	// it's not 100% relible on all systems/drivers
	glfwSwapInterval(0);

	// enable shader program
	program.bind();
	
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

	Camera camera(CameraMode::fpv);
 
	do {

		glm::mat4 model = glm::mat4(1.0f);

		if( last != time(0) ) {
			std::string title = "LibTile3D | FPS: " + std::to_string(count);
			glfwSetWindowTitle(window, title.c_str());
			last = time(0);
			count = 0;
		}

		// fancy location
		glm::mat4 view = camera.update(window);

		// clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// pass matricies to GPU
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		chunkA.render( 0, 0, 0, modelLoc );
		chunkB.render( 4, 0, 4, modelLoc );
		chunkC.render( 4, 0, 0, modelLoc );
		chunkD.render( 0, 0, 4, modelLoc );

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		GLHelper::getError();

		count ++;

	} while( glfwWindowShouldClose(window) == 0 );

	// close window
	glfwTerminate();

	return 0;
}


