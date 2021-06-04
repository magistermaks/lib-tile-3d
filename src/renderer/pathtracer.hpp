#pragma once

#include "../config.hpp"

class Layer;
class Camera;

class PathTracer {

	private:
		
		unsigned int spp, width, height, size;
		
		cl::Kernel kernel;
		cl::Buffer buffer, voxel_buffer, scene_buffer;
		cl::CommandQueue queue;
		cl::NDRange range;

		byte* texture = nullptr;
		byte* octree;
		int octree_depth = 6;

	public:

		PathTracer( int spp, int w, int h, byte* chunk, int octree_depth );
		~PathTracer();

		void resize( int, int );
		void update_buffers( Camera& );
		void render( Layer&, Camera& );

};

