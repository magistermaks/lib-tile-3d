
#require shader/smallpt/math.cl

void kernel simple_add(global const int* A, global const int* B, global int* C) {
	C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];
}

//Ray::Ray( glm::dvec3 o, glm::dvec3 d ) : o(o), d(d) {}
//Sphere::Sphere(double rad, glm::dvec3 p, glm::dvec3 e, glm::dvec3 c, Material mat): rad(rad), p(p), e(e), c(c), refl(mat) {} 
//
//Sphere spheres[] = {//Scene: radius, position, emission, color, material 
//	Sphere( 1e5, glm::dvec3( 1e5+1,40.8,81.6),  glm::dvec3(), glm::dvec3(.75,.25,.25), Material::DIFF), // left 
//	Sphere( 1e5, glm::dvec3(-1e5+99,40.8,81.6), glm::dvec3(), glm::dvec3(.25,.25,.75), Material::DIFF), // right 
//	Sphere( 1e5, glm::dvec3(50,40.8, 1e5),      glm::dvec3(), glm::dvec3(.75,.75,.75), Material::DIFF), // back 
//	Sphere( 1e5, glm::dvec3(50,40.8,-1e5+170),  glm::dvec3(), glm::dvec3(.02,.02,.02), Material::DIFF), // front 
//	Sphere( 1e5, glm::dvec3(50, 1e5, 81.6),     glm::dvec3(), glm::dvec3(.75,.75,.75), Material::DIFF), // bottom 
//	Sphere( 1e5, glm::dvec3(50,-1e5+81.6,81.6), glm::dvec3(), glm::dvec3(.75,.75,.75), Material::DIFF), // top 
//	Sphere(16.5, glm::dvec3(27,16.5,47),        glm::dvec3(), glm::dvec3(1,1,1)*.999,  Material::SPEC), // mirror 
//	Sphere(16.5, glm::dvec3(73,16.5,78),        glm::dvec3(), glm::dvec3(1,1,1)*.999,  Material::REFR), // glass 
//	Sphere( 600, glm::dvec3(50,681.6-.27,81.6), glm::dvec3(12,12,12), glm::dvec3(),    Material::DIFF)  // lamp
//}; 
//
//inline int toInt(double x){ 
//	return int( pow(Math::clamp(x), 5.0/11.0) * 255 + .5 ); 
//}
// 
//inline bool intersect(const Ray &r, double &t, int &id){ 
//	double d; 
//	t = 1e20; 
//
//	for( int i = sizeof(spheres) / sizeof(Sphere); i--; ) {
//		if( ( d = spheres[i].intersect(r) ) && d < t ) {
//			t = d;
//			id = i;
//		}
//	}
//
//	return t < 1e20;; 
//} 
//
//glm::dvec3 radiance(const Ray &r, int depth, unsigned short *Xi){ 
//	
//	// distance to intersection 
//	double t;
//
//	// id of intersected object 
//	int id = 0;
//
//	// if miss, return black 
//	if( !intersect(r, t, id) ) return glm::dvec3();
//
//	// the hit object
//	const Sphere &obj = spheres[id];
//	glm::dvec3 x = r.o + r.d * t, n = glm::normalize(x - obj.p), nl = glm::dot(n, r.d) < 0 ? n : n * -1.0, f = obj.c; 
//
//	// max refl 
//	double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; 
//
//	//R.R. 
//	if (++depth>5) if (erand48(Xi) < p) f = f * (1 / p); else return obj.e; 
//
//	// Ideal DIFFUSE reflection 
//	if( obj.refl == Material::DIFF ){
//		double r1=2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2); 
//		glm::dvec3 w=nl, u= glm::normalize( glm::cross((fabs(w.x)>.1?glm::dvec3(0,1,0):glm::dvec3(1,0,0)), w) ), v= glm::cross(w, u); 
//		glm::dvec3 d = glm::normalize(u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)); 
//		return obj.e + f * (radiance(Ray(x,d),depth,Xi)); 
//
//	// Ideal SPECULAR reflection 
//	} else if( obj.refl == Material::SPEC ) {
//		return obj.e + f * (radiance(Ray(x,r.d-n*2.0*glm::dot(n, r.d)),depth,Xi)); 
//	}
//
//	// Ideal dielectric REFRACTION 
//	Ray reflRay(x, r.d - n * 2.0 * glm::dot(n, r.d));
//
//	// Ray from outside going in? 
//	bool into = glm::dot(n, nl) > 0;
//	double nnt = into ? (1 / 1.5) : (1.5 / 1), ddn = glm::dot(r.d, nl), cos2t; 
//
//	// Total internal reflection 
//	if( ( cos2t = 1 - nnt * nnt * (1 - ddn * ddn) ) < 0) {
//		return obj.e + f * radiance(reflRay, depth, Xi); 
//	}
//	
//	glm::dvec3 tdir = glm::normalize( r.d * nnt - n * ((into?1:-1) * (ddn * nnt + sqrt( cos2t )))); 
//	double c = 1 - (into ? -ddn : glm::dot(tdir, n)); 
//	double Re = 0.04 + 0.96 * c*c*c*c*c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P); 
//
//	// Russian roulette 
//	return obj.e + f * (depth > 2 ? (erand48(Xi) < P
//		? radiance(reflRay, depth, Xi) * RP : radiance(Ray(x,tdir), depth, Xi) * TP)
//		: radiance(reflRay, depth, Xi) * Re + radiance(Ray(x,tdir), depth, Xi) * Tr); 
//} 
//
//void kernel trace( global byte* image ) {
//	
//	// pass as params
//	int samples = 4;
//	
//
//	real rsamples = 1.0 / samples;
//
//	// Loop cols 
//	for( unsigned short x = 0, Xi[3] = {0, 0, (short unsigned) (y*y*y)}; x < w; x ++ ) {
//
//		vec3 pixel;
//
//		// 2x2 subpixel rows 
//		for( int sy = 0, i = y * w + x; sy < 2; sy ++ ) {
//
//			// 2x2 subpixel cols 
//			for( int sx = 0; sx<2; sx++, r=glm::dvec3()) {
//
//				for( int s = 0; s < samples; s ++ ) { 
//
//					double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
//					double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
//
//					glm::dvec3 d = 	cx * ( ( (sx + .5 + dx) / 2 + x) / w - .5) + 
//									cy * ( ( (sy + .5 + dy) / 2 + y) / h - .5) + camera.d; 
//
//					r = r + radiance( Ray( camera.o + d * 140.0, norm(d) ), 0, Xi ) * rsamples; 
//					// Camera rays are pushed ^^^^^ forward to start in interior 
//
//				}
//
//				
//
//				image[i] = image[i] + glm::dvec3(
//					sclamp(r.x), 
//					sclamp(r.y), 
//					sclamp(r.z)
//				) * 0.25;
//
//			}
//		}
//	}
//}
	
