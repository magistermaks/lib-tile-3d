#pragma once

#include "config.hpp"

class Voxel {
	
	public:
	
		byte r;
		byte g;
		byte b;
		byte a;

		Voxel( byte, byte, byte, byte );
		Voxel( const Voxel& );
		Voxel();

		Voxel& operator=( Voxel&& );

		static Voxel random();

};
