#pragma once

#include "../config.hpp"

class Layer;

struct Ray { 
	glm::dvec3 o, d; 

	Ray(glm::dvec3, glm::dvec3);
};

struct Sphere { 

	double rad;         // radius 
	glm::dvec3 p, e, c; // position, emission, color 
	Material refl;      // reflection type (DIFFuse, SPECular, REFRactive) 

	Sphere(double, glm::dvec3, glm::dvec3, glm::dvec3, Material);

	/// returns distance, 0 if nohit
	inline double intersect(const Ray &r) const {

		// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
		glm::dvec3 op = p - r.o;
		double t, b = glm::dot(op, r.d), det = b * b - glm::dot(op, op) + rad * rad; 

		if( det < 0 ) return 0; else det = sqrt(det); 
		return (t = b - det) > 1e-4 ? t : ((t = b + det) > 1e-4 ? t : 0); 

	} 

}; 

class SimpleSpherePathTracer {

	private:
		
		std::vector<Sphere> spheres;
		int spp;

	public:

		SimpleSpherePathTracer( int, std::vector<Sphere> );
		
		void render( Layer&, int, int );

};

