
#include "config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/image/read.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/image/write.h>

//#define WORLD

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
	ShaderProgram* depth = GLHelper::loadShaderProgram("depth");
	ShaderProgram* layer = GLHelper::loadShaderProgram("layer");
	ShaderProgram* lines = GLHelper::loadShaderProgram("lines");

	GLint loc = lines->location("mvp");

	glm::mat4 proj = glm::perspective(glm::radians(77.5f), (float) width / (float) height, 0.1f, 1000.0f);

	PathTracer tracer( 8, width, height, 6, 0 );
	ChunkManager manager( tracer );
	Region region( manager );

	logger::info("Generating voxel data...");

#ifdef WORLD
	Worldgen::gen_chunk_world(region);
#else
	// floating voixels surrounded with cubes
	Worldgen::gen_chunk_scene1(region);
	auto& tree = *region.chunk(0, 0, 0)->tree;
#endif

	Charset charset( "assets/8x8font.png" );

	time_t last = 0;
	long count = 0, fps = 0, ms = 0;

	auto& renderer = RenderSystem::instance();

	VertexConsumerProvider provider;
	provider.attribute(2); // 0 -> [x, y]
	provider.attribute(2); // 1 -> [u, v]

	VertexConsumer consumer = provider.get();
	renderer.setConsumer(consumer);

	VertexConsumerProvider linesProvider;
	linesProvider.attribute(3); // 0 -> [x, y, z]
//	linesProvider.setPrimitive(GL_LINES);

	VertexConsumer linesConsumer = linesProvider.get();

	Camera camera;

	// move the camera so that we don't start inside a black cube
	camera.move( glm::vec3(1, 150, 1) );

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

		renderer.setConsumer(consumer);
		renderer.setShader(*depth);
		manager.update();
		camera.update();
		tracer.render( camera );

		renderer.setShader(*layer);
		renderer.drawText( "FPS: " + std::to_string(fps) + " (avg: " + std::to_string(ms) + "ms)", -1, 1-0.05, 0.04, charset ); 

		lines->bind();
		renderer.setShader(*lines);
		renderer.setConsumer(linesConsumer);
		glm::mat4 model = MatrixStack::getModelIdentity();
		glm::mat4 mvp = proj * camera.getView() * model;

		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

		renderer.vertex3f(  0+10, -10+10,   0-10);
		renderer.vertex3f(  0+10,  10+10,   0-10);
		renderer.vertex3f( 10+10,   0+10,   0-10);
		renderer.vertex3f(-10+10,   0+10,   0-10);
		renderer.vertex3f(  0+10,   0+10,  10-10);
		renderer.vertex3f(  0+10,   0+10, -10-10);
		renderer.draw();

		GLHelper::frame();
		count ++;

		ms = (ms + std::chrono::duration_cast<milliseconds>( Clock::now() - start ).count())/2;

	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	delete depth;
	delete layer;

	// close window
	glfwTerminate();

	return 0;
}


