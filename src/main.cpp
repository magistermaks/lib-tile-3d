
#include "config.hpp"

int main( void ) {

	const int width = 1024;
	const int height = 768;
	bool building = false;

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

	byte* arr1 = Chunk::allocate();
	Chunk::genBall( arr1, 0, 40 );

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders();

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	 
	Region region;

	for( int x = 0; x < 8; x ++ ) {
		for( int y = 0; y < 1; y ++ ) {
			for( int z = 0; z < 8; z ++ ) {
				region.put( arr1, x, y, z );
			}
		}
	}

	logger::info("Generating vertex data...");
	region.build();

	// get locations from shader program
	GLuint modelLoc = program.location("model");
	GLuint viewLoc = program.location("view");
	GLuint projectionLoc = program.location("projection");

	time_t last = 0;
	long count = 0;

	// enable shader program
	program.bind();
	
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

	Camera camera(CameraMode::fpv, window);
 
	do {

		region.update();

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

		region.render( modelLoc );

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		GLHelper::getError();

		count ++;

		if( glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS ) {
			if( !building ) {
				logger::info("Generating vertex data...");
				region.discard();
				region.build();
				building = true;
			}
		}else{
			building = false;
		}

	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	// close window
	glfwTerminate();

	return 0;
}


