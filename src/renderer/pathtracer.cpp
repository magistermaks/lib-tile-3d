
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pathtracer.hpp"


PathTracer::PathTracer( int spp, int w, int h ) {
	this->spp = spp;

	// load the path-tracing kernel, and create OpenCl task queue
	this->kernel = CLHelper::loadKernel( "trace.cl", "render" );
	this->queue = cl::CommandQueue( cl::Context::getDefault(), cl::Device::getDefault() );	

	// initialize all size dependent components
	resize( w, h );
}

PathTracer::~PathTracer() {
	if( this->texture != nullptr ) {
		delete[] this->texture;
	}
}

void PathTracer::resize( int w, int h ) {

	if( this->texture != nullptr ) {
		delete[] this->texture;
	}

	this->width = w;
	this->height = h;
	this->size = w * h * 3 * sizeof(byte);

	this->buffer = cl::Buffer(CL_MEM_WRITE_ONLY, this->size);
	this->range = cl::NDRange(w, h);
	this->texture = Layer::allocate(w, h);

	float scene[] = { 
		-0.5, -0.5, -4.0, // camera origin
		0, 0, 0, // camera direction
		0, 0, 0, // sky light
		0, 0, 0, // ambient light
	};

	byte voxels[] = {
		0
	};

	this->scene_buffer = cl::Buffer(CL_MEM_READ_ONLY, 3 * 4 * sizeof(float));

	// TODO/INFO: size of the (voxel) buffer on the GPU, can be update, but it's expensive
	this->voxel_buffer = cl::Buffer(CL_MEM_READ_ONLY, 1 * sizeof(byte));

	// no need to send this to GPU
	this->kernel.setArg(0, this->spp);
	this->kernel.setArg(1, w);
	this->kernel.setArg(2, this->buffer);
	
	// those ones need to be send every time they change
	this->kernel.setArg(3, scene_buffer);
	this->kernel.setArg(4, voxel_buffer);
	
	// TODO/INFO: send data to buffers in the GPU, do this every time the data changes
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, 3 * 4 * sizeof(float), scene);
	this->queue.enqueueWriteBuffer(voxel_buffer, CL_TRUE, 0, 1 * sizeof(byte), voxels);
 
}

void PathTracer::update_buffers( Camera& camera  ) {
	glm::vec3 cpos = camera.getPosition();
	float scene[3] = { cpos.x, cpos.y, cpos.z };
	this->queue.enqueueWriteBuffer(scene_buffer, CL_TRUE, 0, 3 * 1 * sizeof(float), scene);
}

void PathTracer::render( Layer& layer, Camera& camera ) {

	// add kernel execution to the queue
	this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, this->range, cl::NullRange);

    // read result from the device to the texture
    this->queue.enqueueReadBuffer(this->buffer, CL_TRUE, 0, this->size, this->texture);

	this->update_buffers(camera);

	// draw to screen
	layer.update(this->texture, width, height);

}

