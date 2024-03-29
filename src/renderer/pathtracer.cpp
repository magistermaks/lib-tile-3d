
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pathtracer.hpp"

PathTracer* PathTracer::self = nullptr;

PathTracer::PathTracer( int spp, int w, int h, int octree_depth, byte render_mode ) {
	this->spp = spp;
	this->octree_depth = octree_depth;

	//render_mode 0 - full resolution, 1 - half of pixels, 2 - quarter of pixels
	this->render_mode = render_mode;

	// load the path-tracing kernel, and create OpenCl task queue
	this->kernel = CLHelper::loadKernel("trace");
	this->queue = cl::CommandQueue( cl::Context::getDefault(), cl::Device::getDefault() );	
	this->scene = new Scene();
	this->screen = nullptr;

	// init scene
	scene->setBackground(3, 169, 252);
	scene->setProjection(glm::radians(77.5f), 0.1f, 1000.0f);

	// initialize all size dependent components
	resize( w, h );

	if( PathTracer::self != nullptr ) {
		throw std::runtime_error("PathTracer already in use!");
	}

	PathTracer::self = this;
}

PathTracer::~PathTracer() {
	delete this->scene;
	delete this->screen;

	PathTracer::self = nullptr;
}

PathTracer* PathTracer::instance() {
	return PathTracer::self;
}

void PathTracer::resize( int w, int h ) {

	if( this->screen != nullptr ) {
		delete this->screen;
	}

	this->width = w;
	this->height = h;

	// scale of rendering resolution
	glm::vec2 scale[3] = { {1,1}, {1,2}, {2,2} };
	this->range = cl::NDRange(w / scale[this->render_mode].x, h / scale[this->render_mode].y);

	// texture to draw on
	this->screen = new Screen(w, h);

	this->scene_buffer = cl::Buffer(CL_MEM_READ_ONLY, scene->size());
	this->image_buffer = this->screen->getHandle(CL_MEM_WRITE_ONLY);
	this->object_array = { this->image_buffer };

	// static args
	this->kernel.setArg(0, this->spp);
	this->kernel.setArg(1, w);
	this->kernel.setArg(2, h);
	this->kernel.setArg(3, this->octree_depth);
	this->kernel.setArg(4, this->chunk_count);
	
	// those ones need to be send every time they change
	this->kernel.setArg(5, this->image_buffer);
	this->kernel.setArg(6, this->scene_buffer);
	
	// send data to buffers in the GPU, do this every time the data changes
	this->queue.enqueueWriteBuffer(scene_buffer, LT3D_OPENCL_COPY_ON_WRITE, 0, scene->size(), scene->ptr());
}

void PathTracer::updateCamera( Camera& camera ) {

	auto& pos = camera.getPosition();
	scene->setCameraOrigin( pos.x, pos.y, pos.z );

	auto& rot = camera.getRotation();
	scene->setCameraDirection( rot.x, rot.y, rot.z );

	// send to kernel
	this->queue.enqueueWriteBuffer(scene_buffer, LT3D_OPENCL_COPY_ON_WRITE, 0, scene->size(), scene->ptr());
}

void PathTracer::resizeVoxels( size_t size ) {
	this->voxel_buffer = cl::Buffer( CL_MEM_READ_ONLY, size );
	this->kernel.setArg(7, voxel_buffer);
}

void PathTracer::updateVoxels( size_t offset, size_t count, byte* ptr ) {
	this->queue.enqueueWriteBuffer(voxel_buffer, LT3D_OPENCL_COPY_ON_WRITE, offset, count, ptr);
	this->kernel.setArg(7, voxel_buffer);
}

void PathTracer::updateChunks( size_t count, float* ptr ) {
	this->chunk_count = count;
	this->chunk_buffer = cl::Buffer(CL_MEM_READ_ONLY, count * 3 * sizeof(float));
	this->queue.enqueueWriteBuffer(chunk_buffer, CL_TRUE /* intentional */, 0, count * 3 * sizeof(float), (byte*) ptr);

	this->kernel.setArg(4, chunk_count);
	this->kernel.setArg(8, chunk_buffer);
}

void PathTracer::render( Camera& camera ) {

	static bool pressed = false;

	if( glfwGetKey(GLHelper::window(), GLFW_KEY_TAB) == GLFW_PRESS ) {
		if( !pressed ) {
			if( octree_depth > 1 ) {
				octree_depth --;
			}else{
				octree_depth = 6;
			}

			this->kernel.setArg(3, this->octree_depth);
			pressed = true;
		}
	}else{
		pressed = false;
	}

	// selects pixels which are rendered (if "blur" enabled)
	int top[3] = {0, 1, 3};
	this->kernel.setArg(9, (byte)(pixel + this->render_mode * 8));
	pixel = (pixel >= top[this->render_mode]) ? 0 : pixel + 1;

	auto& renderer = RenderSystem::instance();

	// make sure OpenGL won't try reading from buffers while we modify them
	renderer.flush();

	// update camera
	this->updateCamera(camera);

	// acquire texture handle
	this->queue.enqueueAcquireGLObjects(&object_array);

	// add kernel execution to the queue
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, this->range, cl::NullRange);

    // release texture handle
	this->queue.enqueueReleaseGLObjects(&object_array);

	// draw the screen
	renderer.drawScreen(*screen);

}

