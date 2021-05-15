#pragma once

#include "../config.hpp"

class Layer;

void render( Layer&, int, int, int );

//class SimpleSpherePathTracer {
//
//	private:
//		
//		std::vector<Sphere> spheres;
//		int spp;
//
//	public:
//
//		SimpleSpherePathTracer( int spp, std::vector<Sphere> );
//		~SimpleSpherePathTracer();
//		
//		void render( Layer& layer );
//
//}
