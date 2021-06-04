
#include "config.hpp"

int main() {

	const int width = 1024;
	const int height = 768;
	const int octree_depth = 4; //(value)3 - (edge length)8, 4 - 16, 5 - 32, 6 - 64, 7 - 128, 8 - 256

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

	byte* arr1 = Chunk::allocate( octree_depth );
	//Chunk::genBall( arr1, 0, 40 );

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders( "layer" );

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	 
	Region region;
	Renderer renderer;
	PathTracer tracer( 8, 1024, 768, arr1, octree_depth );

	Layer& layer = renderer.addLayer( 1 );

	/*for( int x = 0; x < 8; x ++ ) {
		for( int y = 0; y < 1; y ++ ) {
			for( int z = 0; z < 8; z ++ ) {
				region.put( arr1, x, y, z );
			}
		}
	}*/

	time_t last = 0;
	long count = 0;

	// enable shader program
	program.bind();

	Camera camera(CameraMode::fpv, window);
 
	do {

		if( last != time(0) ) {
			std::string title = "LibTile3D | FPS: " + std::to_string(count);
			glfwSetWindowTitle(window, title.c_str());
			last = time(0);
			count = 0;
		}

		camera.update( window );
		tracer.render( layer, camera );
		renderer.render();

		GLHelper::frame();
		count ++;

	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	// close window
	glfwTerminate();

	return 0;
}


