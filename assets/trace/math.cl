
// old habits die hard
#define nullptr 0

// define byte type for convenience
typedef unsigned char byte;

// define 3D vector
typedef struct {
	float x, y, z;
} Vec3;

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

// load vector from float array
void load_vec3( Vec3* v, global float* arr ) {
	v->x = (float) arr[0];
	v->y = (float) arr[1];
	v->z = (float) arr[2];
}

// add two vectors, and returns the resulting vector
inline Vec3 add( Vec3* a, Vec3* b ) {
	Vec3 c;
	vvopt( a, b, &c, + );
	return c;
}

// add vector and scalar, and returns the resulting vector
inline Vec3 adds( Vec3* a, float b ) {
	Vec3 c;
	vsopt( a, b, &c, + );
	return c;
}

// substracts vector b from vector a, and returns the resulting vector
inline Vec3 sub( Vec3* a, Vec3* b ) {
	Vec3 c;
	vvopt( a, b, &c, - );
	return c;
}

// mutliplies two vectors, and returns the resulting vector
inline Vec3 mul( Vec3* a, Vec3* b ) {
	Vec3 c;
	vvopt( a, b, &c, * );
	return c;
}

// mutliplies vector and scalar, and returns the resulting vector
inline Vec3 muls( Vec3* a, float b ) {
	Vec3 c;
	vsopt( a, b, &c, * );
	return c;
}

// normalize given vector, returns given pointer
inline Vec3* norm( Vec3* a ) {
	const float b = ( 
		1.0 / sqrt( a->x * a->x + a->y * a->y + a->z * a->z )
	);

	vsopt( a, b, a, * );
	return a; 
}

// get the dot product of two given vectors
inline float vdot( Vec3* a, Vec3* b ) {
	return a->x * b->x + a->y * b->y + a->z * b->z; 
}

// clamps given float to range [0-1]
inline float sclamp( float x ){ 
	return x < 0 ? 0 : x > 1 ? 1 : x; 
}

