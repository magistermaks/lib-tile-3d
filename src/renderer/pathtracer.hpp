#pragma once

#include "../config.hpp"

class Canvas;
class Camera;
class Scene;

class PathTracer {

	private:
		
		unsigned int spp, width, height, size;
		
		cl::Kernel kernel;

		cl::Buffer voxel_buffer;
		cl::Buffer scene_buffer;
		cl::Image2DGL image_buffer; 
		std::vector<cl::Memory> object_array;

		cl::CommandQueue queue;
		cl::NDRange range;

		byte* texture = nullptr;
		byte* octree;
		int octree_depth = 6;

		Scene* scene;
		Canvas* canvas;

	public:

		PathTracer( int spp, int w, int h, byte* chunk, int octree_depth );
		~PathTracer();

		void resize( int, int );
		void updateCamera( Camera& );
		void render( Camera& );

};

