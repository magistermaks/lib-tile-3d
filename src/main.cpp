
#include "config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/image/read.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/image/write.h>

#define WORLD

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

	// compile shader program
	ShaderProgram* program = GLHelper::loadShaderProgram("layer");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	 
	logger::info("Generating voxel data...");

	PathTracer tracer( 8, width, height, 6 );
	ChunkManager manager( tracer );

	Region region( manager );

#ifdef WORLD
	Worldgen::gen_chunk_world(region);
#else
	// floating voixels surrounded with cubes
	Worldgen::gen_chunk_scene1(region);
	VoxelTree& tree = *region.chunk(0, 0, 0)->tree;
#endif

	Charset charset( "assets/8x8font.png" );

	time_t last = 0;
	long count = 0, fps = 0, ms = 0;

	// enable shader program
	auto& renderer = RenderSystem::instance();
	renderer.setShader(*program);

	Camera camera;

	// move the camera so that we don't start inside a black cube
	camera.move( glm::vec3(1, 150, 1) );
 
	//size_t c = 0;

	do {

		auto start = Clock::now();

#ifndef WORLD
		tree.set(rand() % 65, rand() % 65, rand() % 65, {
			((byte)rand()), ((byte)rand()), ((byte)rand()), 255
	} );
#endif // !WORLD

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

	delete program;

	// close window
	glfwTerminate();

	return 0;
}


