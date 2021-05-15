#pragma once

#include "../config.hpp"

class Layer {

	private:

		GLuint vbo, vao, tex;

		void genBuffer( float );

	public:

		Layer( float );
		~Layer();
	
		void update( byte*, int, int, bool free = false );
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
