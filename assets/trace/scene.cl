
#require math.cl

// define struct holding scene properties
typedef struct {
	Vec3 camera_origin;
	Vec3 camera_direction;
	Vec3 ambient_light;
	Vec3 sky_light;
	Vec3 background;
} Scene;

// load scene from float array
void load_scene( Scene* s, global float* arr ) {
	load_vec3( &(s->camera_origin),    arr + 0 * 3 );
	load_vec3( &(s->camera_direction), arr + 1 * 3 );
	load_vec3( &(s->ambient_light),    arr + 2 * 3 );
	load_vec3( &(s->sky_light),        arr + 3 * 3 );
	load_vec3( &(s->background),       arr + 4 * 3 );
}

