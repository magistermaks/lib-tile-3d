
#include "voxel.hpp"

Voxel::Voxel( byte r, byte g, byte b, byte a ) : r(r), g(g), b(b), a(a) {
	
}

Voxel::Voxel() : r(0), g(0), b(0), a(0) {
	
}

Voxel::Voxel( const Voxel& vox ) : r(vox.r), g(vox.g), b(vox.b), a(vox.a) {
	
}

Voxel& Voxel::operator=( Voxel&& vox ) {
	r = vox.r;
	g = vox.g;
	b = vox.b;
	a = vox.a;
	return *this;
}
