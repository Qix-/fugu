#include "fg/functions.h"

#include <cstdlib>
#include <cmath>

#include <vcg/math/perlin_noise.h>

namespace fg {

	double noise(double x){return noise(x,0,0);}
	double noise(double x, double y){return noise(x,y,0);}
	double noise(double x, double y, double z){return vcg::math::Perlin::Noise(x,y,z);}

	double random(){
		return (1.0*std::rand())/RAND_MAX; // srand is called when a Universe is created
	}

	double random(double low, double high){
		return fg::random()*(high-low) + low;
	}

	bool approx(double a, double b, double epsilon){
		return (std::abs(a-b) < epsilon);
	}

	bool approx(const Vec3& a, const Vec3& b, double epsilon){
		return approx(a.getX(),b.getX(),epsilon) && approx(a.getY(),b.getY(),epsilon) && approx(a.getZ(),b.getZ(),epsilon);
	}
}
