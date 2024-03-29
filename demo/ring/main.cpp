#include <core.hpp>
#include "worldgen.hpp"

int main() {

	const int width = 1024;
	const int height = 768;

	// print cwd, nice for debugging
	logger::info("Current working directory: '", std::filesystem::current_path().string(), "'");

	// initilize GLFW, GLEW, OpenGL, and OpenCL
	if( !GLHelper::init(width, height, "Ring Demo") ) {
		return -1;
	}
	
	GLFWwindow* window = GLHelper::window();

	// compile shader program
	ShaderProgram* depth = GLHelper::loadShaderProgram("depth");
	ShaderProgram* layer = GLHelper::loadShaderProgram("layer");
	ShaderProgram* mesh = GLHelper::loadShaderProgram("mesh");

	GLint loc = mesh->location("mvp");

	glm::mat4 proj = glm::perspective(glm::radians(77.5f), (float) width / (float) height, 0.1f, 1000.0f);

	PathTracer tracer( 8, width, height, 6, 0 );
	World world( tracer );

	worldgen(world);

	Charset charset( "assets/8x8font.png" );

	time_t last = 0;
	long count = 0, fps = 0, ms = 0;

	auto& renderer = RenderSystem::instance();

	Texture* box = Texture::fromFile("./assets/box.png");

	VertexConsumerProvider provider2d;
	provider2d.attribute(2); // 0 -> [x, y]
	provider2d.attribute(2); // 1 -> [u, v]

	VertexConsumer consumer2d = provider2d.get();
	renderer.setConsumer(consumer2d);

	VertexConsumerProvider provider3d;
	provider3d.attribute(3); // 0 -> [x, y, z]
	provider3d.attribute(2); // 1 -> [u, v]

	VertexConsumer consumer3d = provider3d.get();

	// textured cube
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 0.0f );
	consumer3d.vertex( -0.5f,  0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex(  0.5f, -0.5f, -0.5f,  1.0f, 0.0f );
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 0.0f );
	consumer3d.vertex( -0.5f, -0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex(  0.5f, -0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 1.0f );
	consumer3d.vertex( -0.5f,  0.5f,  0.5f,  0.0f, 1.0f );
	consumer3d.vertex( -0.5f, -0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex( -0.5f,  0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex( -0.5f,  0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex( -0.5f, -0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex( -0.5f,  0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex(  0.5f, -0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex(  0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex(  0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex(  0.5f, -0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex(  0.5f, -0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex(  0.5f, -0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex( -0.5f, -0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex( -0.5f, -0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 0.0f );
	consumer3d.vertex(  0.5f,  0.5f, -0.5f,  1.0f, 1.0f );
	consumer3d.vertex( -0.5f,  0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex( -0.5f,  0.5f, -0.5f,  0.0f, 1.0f );
	consumer3d.vertex( -0.5f,  0.5f,  0.5f,  0.0f, 0.0f );
	consumer3d.vertex(  0.5f,  0.5f,  0.5f,  1.0f, 0.0f );

	Camera camera;

	// move the camera so that we don't start inside a black cube
	camera.move( glm::vec3(1, 150, 1) );

	do {

		auto start = Clock::now();

		Threads::execute();

		Chunk* chunk = world.get(0, 0, 0);

		chunk->tree->set(rand() % 65, rand() % 65, rand() % 65, {
			((byte)rand()), ((byte)rand()), ((byte)rand()), 255
		} );

		chunk->markDirty();

		// update the fps count
		if( last != time(0) ) {
			last = time(0);
			fps = count;
			count = 0;
		}

		renderer.setConsumer(consumer2d);
		renderer.setShader(*depth);
		renderer.depthTest(false);
		world.update();
		camera.update();
		tracer.render( camera );

		renderer.depthMask(false);
		renderer.setShader(*layer);
		renderer.drawText( "FPS: " + std::to_string(fps) + " (avg: " + std::to_string(ms) + "ms)", -1, 1-0.05, 0.04, charset ); 

		renderer.depthMask(true);
		renderer.depthTest(true);

		mesh->bind(); // uniform
		renderer.setShader(*mesh);
		renderer.setConsumer(consumer3d);
		renderer.setTexture(*box);
		glm::mat4 model = MatrixHelper::getVoxelIdentity();
		model = glm::scale(model, glm::vec3(10, 10, 10));
		model = glm::translate(model, glm::vec3(1, 1, -1));

		glm::mat4 mvp = proj * camera.getView() * model;
		MatrixHelper::uniform(loc, mvp);
		renderer.draw();

		GLHelper::frame();
		count ++;

		ms = (ms + std::chrono::duration_cast<milliseconds>( Clock::now() - start ).count())/2;

	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	delete depth, layer, mesh;

	return 0;
}

