
// require doubles
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

// select floating point value to use, float or double
#define real double

// define byte type for convenience
typedef unsigned char byte;

// define 3D (xyz) vector
typedef struct {
	real x, y, z;
} vec3;

// macro used for generic vector-vector math
#define vvopt( ap, bp, out, opt ) \
	(out)->x = (ap)->x opt (bp)->x; \
	(out)->y = (ap)->y opt (bp)->y; \
	(out)->z = (ap)->z opt (bp)->z; \

// macro used for generic vector-scalar math
#define vsopt( ap, b, out, opt ) \
	(out)->x = (ap)->x opt b; \
	(out)->y = (ap)->y opt b; \
	(out)->z = (ap)->z opt b; \

// add two vectors, and returns the resulting vector
inline vec3 add( vec3* a, vec3* b ) {
	vec3 c;
	vvopt( a, b, &c, + );
	return c;
}

// substracts vector b from vector a, and returns the resulting vector
inline vec3 sub( vec3* a, vec3* b ) {
	vec3 c;
	vvopt( a, b, &c, - );
	return c;
}

// mutliplies two vectors, and returns the resulting vector
inline vec3 mul( vec3* a, vec3* b ) {
	vec3 c;
	vvopt( a, b, &c, * );
	return c;
}

// mutliplies vector and scalar, and returns the resulting vector
inline vec3 muls( vec3* a, real b ) {
	vec3 c;
	vsopt( a, b, &c, * );
	return c;
}

// normalize given vector, returns given pointer
inline vec3* norm( vec3* a ) {
	const real b = ( 
		1.0 / sqrt( a->x * a->x + a->y * a->y + a->z * a->z )
	);

	vsopt( a, b, a, * );
	return a; 
}

// get the dot product of two given vectors
inline real vdot( vec3* a, vec3* b ) {
	return a->x * b->x + a->y * b->y + a->z * b->z; 
}

// clamps given real to range [0-1]
inline real sclamp( real x ){ 
	return x < 0 ? 0 : x > 1 ? 1 : x; 
}

