#include "fg/functions.h"

#include <cmath>
#include <vcg/math/perlin_noise.h>

namespace fg {

	double noise(double x){return noise(x,0,0);}
	double noise(double x, double y){return noise(x,y,0);}
	double noise(double x, double y, double z){return vcg::math::Perlin::Noise(x,y,z);}
}
