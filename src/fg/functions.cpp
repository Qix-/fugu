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
    
    /**
     * fractal sum - returns summed noise function of specified octaves
     */
    double fracSum(double x, double y, double z, int nOctaves, double falloff)
    {
        double sum = noise(x, y, z); // first octave
        double oct = 2.0;
        for (int i = 2; i < nOctaves; ++i)
        {
            sum += (noise(x * oct, y * oct, z * oct) * falloff);
            falloff /= 2.0;
            oct *= 2.0;
        }
        return sum;
    }
    
    /**
     * turbulence -- summed octaves of abs(noise)
     * "turbulence forms the primordial pattern, the chaos that was 'in the beginning'" -- Peter S. Stevens
     */
    double turbulence(double x, double y, double z, int nOctaves, double falloff)
    {
        double sum = abs(noise(x, y, z)); // first octave
        double oct = 2.0;
        for (int i = 2; i < nOctaves; ++i)
        {
            sum += (abs(noise(x * oct, y * oct, z * oct)) * falloff);
            falloff /= 2.0;
            oct *= 2.0;
        }
        return sum;
    }
    
    
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
    
    /* return a random number with a normally distributed distribution
     * uses the Box-Muller method
     */
    double randomN(double mean, double sd)
    {
        double x1, x2, w, y1;
        static double y2;
        static int use_last = 0;
        
        if (use_last)		        /* use value from previous call */
        {
            y1 = y2;
            use_last = 0;
        }
        else
        {
            do {
                x1 = 2.0 * random() - 1.0;
                x2 = 2.0 * random() - 1.0;
                w = x1 * x1 + x2 * x2;
            } while ( w >= 1.0 );
            
            w = sqrt( (-2.0 * log( w ) ) / w );
            y1 = x1 * w;
            y2 = x2 * w;
            use_last = 1;
        }
        
        return( mean + y1 * sd );
    }

	bool approx(double a, double b, double epsilon){
		return (std::abs(a-b) < epsilon);
	}

	bool approx(const Vec3& a, const Vec3& b, double epsilon){
		return approx(a.getX(),b.getX(),epsilon) && approx(a.getY(),b.getY(),epsilon) && approx(a.getZ(),b.getZ(),epsilon);
	}
}
