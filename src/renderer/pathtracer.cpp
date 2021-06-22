
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

	this->buffer = cl::Buffer(CL_MEM_WRITE_ONLY, this->size);
	this->range = cl::NDRange(w, h);

	// texture to draw on
	this->texture = Canvas::allocate(w, h);
	this->canvas = new Canvas(w, h);

	this->scene_buffer = cl::Buffer(CL_MEM_READ_ONLY, scene->size());

	// size of the (voxel) buffer on the GPU, can be update, but it's expensive
	const int len = (1 - pow(8, (this->octree_depth + 1))) / -7;
	this->voxel_buffer = cl::Buffer(CL_MEM_READ_ONLY, len * 4 * sizeof(byte));

	// static args
	this->kernel.setArg(0, this->spp);
	this->kernel.setArg(1, w);
	this->kernel.setArg(2, this->buffer);
	this->kernel.setArg(5, this->octree_depth);
	
	// those ones need to be send every time they change
	this->kernel.setArg(3, scene_buffer);
	this->kernel.setArg(4, voxel_buffer);
	
	// send data to buffers in the GPU, do this every time the data changes
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, scene->size(), scene->ptr());
	this->queue.enqueueWriteBuffer(voxel_buffer, CL_TRUE, 0, len * 4 * sizeof(byte), this->octree);
}

void PathTracer::updateCamera( Camera& camera ) {

	// update origin
	glm::vec3 cam = camera.getPosition();
	scene->setCameraOrigin( cam.x, cam.y, cam.z );
	cam = camera.getRotation();
	scene->setCameraDirection( cam.x, cam.y, cam.z );

	// send to GPU
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, scene->size(), scene->ptr());
}

void PathTracer::render( Camera& camera ) {

	// update camera
	this->updateCamera(camera);

	// add kernel execution to the queue
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, this->range, cl::NullRange);

    // read result from the device to the texture
    this->queue.enqueueReadBuffer(this->buffer, CL_TRUE, 0, this->size, this->texture);

	// update texture
	this->canvas->update(this->texture);

	// draw
	RenderSystem::instance().drawScreen(*canvas);

}

