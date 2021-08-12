
#include "config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/image.h>

int main() {

	const int width = 1024;
	const int height = 768;

	// print cwd, nice for debugging
	char tmp[ CWD_MAX_PATH ];
	logger::info("Current working directory: '" + std::string( POSIX_GETCWD(tmp, sizeof(tmp)) ? tmp : "" ) + "'");

	// initilize GLFW, GLEW, OpenGL, and OpenCL
	if( !GLHelper::init(width, height, "lib-tile-3d") ) {
		return -1;
	}
	
	GLFWwindow* window = GLHelper::window();

	// compile GLSL program from the shaders
	GLHelper::ShaderProgram program = GLHelper::loadShaders( "layer" );

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	 
	logger::info("Generating voxel data...");

	PathTracer tracer( 8, width, height, 6 );
	ChunkManager manager( tracer );

	Region region( manager );
	region.put( nullptr, 0, 0, 0 );
	region.put( nullptr, 1, 0, 0 );

	VoxelTree& tree = *region.chunk(0, 0, 0)->tree;

	tree.set(0,0,0,{255, 0, 0, 255});
	tree.set(1,1,0,{0, 255, 0, 255});
	tree.set(2,2,0,{0, 0, 255, 255});
	tree.set(3,3,0,{255, 255, 0, 255});
	tree.set(4,4,0,{0, 255, 255, 255});
	tree.set(5,5,0,{255, 0, 255, 255});

	VoxelTree& tree2 = *region.chunk(1, 0, 0)->tree;

	for( int x = 0; x < 64; x ++ ) {
		for( int y = 0; y < 64; y ++ ) {
			for( int z = 0; z < 64; z ++ ) {
				tree2.set(x, y, z, {255, 255, 255, 255});
			}
		}
	}

	Charset charset( "assets/8x8font.png" );

	/*for( int x = 0; x < 8; x ++ ) {
		for( int y = 0; y < 1; y ++ ) {
			for( int z = 0; z < 8; z ++ ) {
				region.put( arr1, x, y, z );
			}
		}
	}*/

	time_t last = 0;
	long count = 0, fps = 0, ms = 0;

	// enable shader program
	auto& renderer = RenderSystem::instance();
	renderer.setShader(program);

	Camera camera;

	// move the camera so that we don't start inside a black cube
	camera.move( glm::vec3(1, 1, -10) );
 
	//size_t c = 0;

	do {

		auto start = Clock::now();

		//if( c < 100 * 10 ) {
		tree.set( rand() % 65, rand() % 65, rand() % 65, {
			((byte) rand()), ((byte) rand()), ((byte) rand()), 255
		} ); 

		//c ++; }

		// update the fps count
		if( last != time(0) ) {
			last = time(0);
			fps = count;
			count = 0;
		}

		manager.update();
		camera.update();
		tracer.render( camera );

		renderer.drawText( "FPS: " + std::to_string(fps) + " (avg: " + std::to_string(ms) + "ms)", -1, 1-0.05, 0.04, charset ); 

		GLHelper::frame();
		count ++;

		ms = (ms + std::chrono::duration_cast<milliseconds>( Clock::now() - start ).count())/2;

	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	// close window
	glfwTerminate();

	return 0;
}


