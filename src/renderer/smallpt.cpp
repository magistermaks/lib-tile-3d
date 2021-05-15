
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "smallpt.hpp"

/// position, or color
struct Vec {      

	double x, y, z;

	inline Vec(double x_=0, double y_=0, double z_=0){ 
		x=x_; 
		y=y_; 
		z=z_; 
	} 

	inline Vec operator+(const Vec &b) const { 
		return Vec(x + b.x, y + b.y, z + b.z); 
	}
 
	inline Vec operator-(const Vec &b) const { 
		return Vec(x-b.x,y-b.y,z-b.z); 
	} 

	inline Vec operator*(double b) const { 
		return Vec(x*b,y*b,z*b); 
	} 

	inline Vec mult(const Vec &b) const { 
		return Vec(x*b.x,y*b.y,z*b.z); 
	} 

	inline Vec& norm(){ 
		return *this = *this * ( 1 / sqrt(x*x + y*y + z*z)); 
	} 

	inline double dot(const Vec &b) const { 
		return x * b.x + y * b.y + z * b.z; 
	}

	inline Vec operator%(Vec&b){
		return Vec( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	} 
}; 

/// ray struct
struct Ray { 
	Vec o, d; 

	Ray(Vec o_, Vec d_) : o(o_), d(d_) {} 
}; 

/// material types, used in radiance() 
enum Refl_t { 
	DIFF, 
	SPEC, 
	REFR 
};

/// sphere struct
struct Sphere { 
	double rad;       // radius 
	Vec p, e, c;      // position, emission, color 
	Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive) 

	Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_): rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {} 

	/// returns distance, 0 if nohit
	inline double intersect(const Ray &r) const { 
		// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
		Vec op = p - r.o;
		double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + rad*rad; 

		if( det < 0 ) return 0; else det = sqrt(det); 
		return (t = b-det) > eps ? t : ((t = b+det) > eps ? t : 0); 

	} 
}; 

Sphere spheres[] = {//Scene: radius, position, emission, color, material 
	Sphere( 1e5, Vec( 1e5+1,40.8,81.6),  Vec(), Vec(.75,.25,.25), DIFF), // left 
	Sphere( 1e5, Vec(-1e5+99,40.8,81.6), Vec(), Vec(.25,.25,.75), DIFF), // right 
	Sphere( 1e5, Vec(50,40.8, 1e5),      Vec(), Vec(.75,.75,.75), DIFF), // back 
	Sphere( 1e5, Vec(50,40.8,-1e5+170),  Vec(), Vec(.02,.02,.02), DIFF), // front 
	Sphere( 1e5, Vec(50, 1e5, 81.6),     Vec(), Vec(.75,.75,.75), DIFF), // bottom 
	Sphere( 1e5, Vec(50,-1e5+81.6,81.6), Vec(), Vec(.75,.75,.75), DIFF), // top 
	Sphere(16.5, Vec(27,16.5,47),        Vec(), Vec(1,1,1)*.999,  SPEC), // mirror 
	Sphere(16.5, Vec(73,16.5,78),        Vec(), Vec(1,1,1)*.999,  REFR), // glass 
	Sphere( 600, Vec(50,681.6-.27,81.6), Vec(12,12,12), Vec(),    DIFF)  // lamp
}; 

inline int toInt(double x){ 
	return int( pow(Math::clamp(x), 5.0/11.0) * 255 + .5 ); 
}
 
inline bool intersect(const Ray &r, double &t, int &id){ 
	double n = sizeof(spheres) / sizeof(Sphere), d, inf = t = 1e20; 
	for( int i = int(n); i--; ) if((d = spheres[i].intersect(r) ) && d < t){t = d;id = i;} 
	return t < inf; 
} 

Vec radiance(const Ray &r, int depth, unsigned short *Xi){ 
	
	// distance to intersection 
	double t;

	// id of intersected object 
	int id = 0;

	// if miss, return black 
	if( !intersect(r, t, id) ) return Vec();

	// the hit object
	const Sphere &obj = spheres[id];
	Vec x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c; 

	// max refl 
	double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; 

	//R.R. 
	if (++depth>5) if (erand48(Xi)<p) f=f*(1/p); else return obj.e; 

	// Ideal DIFFUSE reflection 
	if (obj.refl == DIFF){
		double r1=2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2); 
		Vec w=nl, u=((fabs(w.x)>.1?Vec(0,1):Vec(1))%w).norm(), v=w%u; 
		Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm(); 
		return obj.e + f.mult(radiance(Ray(x,d),depth,Xi)); 

	// Ideal SPECULAR reflection 
	} else if (obj.refl == SPEC) {
		return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi)); 
	}

	// Ideal dielectric REFRACTION 
	Ray reflRay(x, r.d-n*2*n.dot(r.d));

	// Ray from outside going in? 
	bool into = n.dot(nl)>0;
	double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t; 

	// Total internal reflection 
	if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0) {
		return obj.e + f.mult(radiance(reflRay,depth,Xi)); 
	}
	
	Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm(); 
	double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n)); 
	double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P); 

	// Russian roulette 
	return obj.e + f.mult(depth > 2 ? (erand48(Xi) < P
		? radiance(reflRay, depth, Xi) * RP : radiance(Ray(x,tdir), depth, Xi) * TP)
		: radiance(reflRay, depth, Xi) * Re + radiance(Ray(x,tdir), depth, Xi) * Tr); 
} 

void render( Layer& layer, int w, int h, int spp ) {

	// # samples 
	int samps = spp/4;

	// cam pos, dir 
	Ray cam(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm());
	Vec cx=Vec(w*.5135/h), cy=(cx%cam.d).norm()*.5135, r, *c=new Vec[w*h];

	// OpenMP, loop over image rows 
#	pragma omp parallel for schedule(dynamic, 1) private(r)
	for (int y=0; y<h; y++) {

		fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*y/(h-1)); 

		// Loop cols 
		for (unsigned short x=0, Xi[3]={0, 0, (short unsigned) (y*y*y)}; x<w; x++) {

			// 2x2 subpixel rows 
			for (int sy=0, i=y*w+x; sy<2; sy++) {

				// 2x2 subpixel cols 
				for (int sx=0; sx<2; sx++, r=Vec()) {

					for (int s=0; s<samps; s++){ 
						double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1); 
						double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2); 
						Vec d = cx*( ( (sx+.5 + dx)/2 + x)/w - .5) + 
						cy*( ( (sy+.5 + dy)/2 + y)/h - .5) + cam.d; 
						r = r + radiance(Ray(cam.o+d*140,d.norm()),0,Xi)*(1./samps); 
					} // Camera rays are pushed ^^^^^ forward to start in interior 

					c[i] = c[i] + Vec(Math::clamp(r.x), Math::clamp(r.y), Math::clamp(r.z))*.25; 		
				}
			}
		}
	}

	byte* image = Layer::allocate( w, h );

	for( int i = 0; i < w*h; i ++ ) {
		image[i * 3 + 0] = toInt(c[i].x);
		image[i * 3 + 1] = toInt(c[i].y);
		image[i * 3 + 2] = toInt(c[i].z);
	}

	delete[] c;

	layer.update( image, w, h, true );

}


