#pragma once

#include <config.hpp>

class PathTracer {

	private:
		static PathTracer* self;
		
		unsigned int spp, width, height;
		byte render_mode = 0;
		byte pixel = 0;
		
		// OpenCL compute kernel
		cl::Kernel kernel;

		// input/output buffers
		cl::Buffer voxel_buffer;
		cl::Buffer scene_buffer;
		cl::Buffer chunk_buffer;
		cl::Image2DGL image_buffer; 
		std::vector<cl::Memory> object_array;

		cl::CommandQueue queue;
		cl::NDRange range;

		int octree_depth = 6, chunk_count = 0;

		Scene* scene;
		Screen* screen;

	public:

		PathTracer( int spp, int w, int h, int octree_depth, byte render_mode );
		~PathTracer();

		static PathTracer* instance();

		void resizeVoxels( size_t size );
		void updateVoxels( size_t offset, size_t count, byte* ptr );
		void updateChunks( size_t count, float* ptr );

		void resize( int, int );
		void updateCamera( Camera& );
		void render( Camera& );

};

