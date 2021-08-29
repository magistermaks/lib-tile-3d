#pragma once

#include "../config.hpp"

class Scene {

	private:

		float buffer[3 * 6] = {
			0, 0, 0, // camera origin
			0, 0, 0, // camera direction
			0, 0, 0, // sky light
			0, 0, 0, // ambient light
			0, 0, 0, // background color
			0, 0, 0, // fov, near plane, far plane
		};

		void set( int, float, float, float );

	public:

		void setCameraOrigin( float, float, float );
		void setCameraDirection( float, float, float );
		void setSkyLight( float, float, float );
		void setAmbientLight( float, float, float );
		void setBackground( float, float, float );
		void setProjection( float, float, float );

		float* ptr();
		size_t size();

};

