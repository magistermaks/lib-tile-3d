
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pathtracer.hpp"

PathTracer::PathTracer( int spp, int w, int h, byte* chunk, int octree_depth ) {
	this->spp = spp;
	this->octree = chunk;
	this->octree_depth = octree_depth;

	// load the path-tracing kernel, and create OpenCl task queue
	this->kernel = CLHelper::loadKernel( "trace.cl", "render" );
	this->queue = cl::CommandQueue( cl::Context::getDefault(), cl::Device::getDefault() );	
	this->scene = new Scene();
	this->canvas = nullptr;

	// init scene
	scene->setBackground( 0, 0, 102 );

	// initialize all size dependent components
	resize( w, h );
}

PathTracer::~PathTracer() {
	if( this->texture != nullptr ) {
		delete[] this->texture;
	}

	delete this->scene;
	delete this->canvas;
}

void PathTracer::resize( int w, int h ) {

	if( this->texture != nullptr ) {
		delete[] this->texture;
	}

	if( this->canvas != nullptr ) {
		delete this->canvas;
	}

	this->width = w;
	this->height = h;
	this->size = w * h * 3 * sizeof(byte);
	this->range = cl::NDRange(w, h);

	// texture to draw on
	this->texture = Canvas::allocate(w, h);
	this->canvas = new Canvas(w, h);
	this->canvas->update(this->texture);

	this->scene_buffer = cl::Buffer(CL_MEM_READ_ONLY, scene->size());

	// size of the (voxel) buffer on the GPU, can be update, but it's expensive
	const int len = (1 - pow(8, (this->octree_depth + 1))) / -7;
	this->voxel_buffer = cl::Buffer(CL_MEM_READ_ONLY, len * 4 * sizeof(byte));

	this->image_buffer = cl::Image2DGL( cl::Context::getDefault(), CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, this->canvas->id() );
	this->object_array = { this->image_buffer };

	// static args
	this->kernel.setArg(0, this->spp);
	this->kernel.setArg(1, w);
	this->kernel.setArg(2, h);
	this->kernel.setArg(3, this->image_buffer);
	this->kernel.setArg(6, this->octree_depth);
	
	// those ones need to be send every time they change
	this->kernel.setArg(4, scene_buffer);
	this->kernel.setArg(5, voxel_buffer);
	
	// send data to buffers in the GPU, do this every time the data changes
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, scene->size(), scene->ptr());
	this->queue.enqueueWriteBuffer(voxel_buffer, CL_TRUE, 0, len * 4 * sizeof(byte), this->octree);
}

void PathTracer::updateCamera( Camera& camera ) {

	auto& pos = camera.getPosition();
	scene->setCameraOrigin( pos.x, pos.y, pos.z );

	auto& rot = camera.getRotation();
	scene->setCameraDirection( rot.x, rot.y, rot.z );

	// send to kernel
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, scene->size(), scene->ptr());
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

			this->kernel.setArg(6, this->octree_depth);
			pressed = true;
		}
	}else{
		pressed = false;
	}

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

	// draw to screen
	RenderSystem::instance().drawScreen(*canvas);

}

