/**
 * \file
 * \author ben jonmc
 * 
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___     
 *   |  _|___ 
 *   |  _| . | fg: real-time procedural 
 *   |_| |_  | animation and generation 
 *       |___| of 3D forms
 *
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in 
 *   the LICENSE file.
 * 
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */


 
#include "fg/functions.h"

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>


#include <vcg/math/perlin_noise.h>

namespace fg {
        
    /**
     * noise in 1, 2 and 3D
     * 1D and 2D just wrap to 3D noise
     */
	double noise(double x){return noise(x,0,0);}
	double noise(double x, double y){return noise(x,y,0);}
	double noise(double x, double y, double z){return vcg::math::Perlin::Noise(x,y,z);}

    /*
     * random convience function uses std::rand() which is not the best RNG
     */
	double random(){
		return (1.0*std::rand())/RAND_MAX; // srand is called when a Universe is created
	}
    
    /* return a random in the range [low,high]
     *
     */ 
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
