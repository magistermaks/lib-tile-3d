#pragma once

#include "../config.hpp"

class Layer {

	private:

		GLuint vbo, vao, tex;
		bool fresh = true;

		void genBuffer( float );

	public:

		Layer( float );
		~Layer();
	
		void update( byte*, int, int );
		void render();

		static byte* allocate( int, int );

};

class Renderer {

	private:

		std::vector<Layer> layers;

	public:

		Layer& addLayer( float );
		void render();

};
