#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

/**
 * Useful mathematical and geometric functions.
 */

#include <cfloat>

#include "fg/vec3.h"
#include "fg/mat4.h"

namespace fg {
        
    const double EPSILON = DBL_EPSILON; // A small double..

	template <class T> 
    T min(const T& a, const T& b);
    
	template <class T, typename Real> 
    T lerp(const T& a, const T& b, Real t);
    
    template <class T, typename Real>
	T mix(const T& a, const T& b, Real mixValue);
    
	template <class T>
	T clamp(const T& x, const T& a, const T& b);
    
	template <class T, typename Real >
	Real step(const T& a, const T& x);
    
	template <class T, typename Real >
	Real pulse(const T& a, const T&b, const T& x);
    
	template <typename Real>
	Real smoothstep(Real a, Real b, Real x);
    
	template <class Real>
	Real spline(Real x, int knots, Real * knot);
    
	template <class Real>
	Real bias(Real b, Real x);
    
	template <class Real>
	Real fastBias(Real b, Real x);
    
	template <class Real>
	Real gain(Real g, Real x);
    
	template <class Real>
	Real fastGain(Real g, Real x);
    
	template <class Real>
	Real gammaCorrect(Real gamma, Real x);


    /**
     * noise functions currently use vcg's Perlin noise function
     */
	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

    /**
     * random currently uses the OS rand function (beware!)
     */
	double random(); ///< Returns a random number between 0 and 1 inclusive
	double random(double low, double high); ///< Returns a random number between low and high inclusive

	bool approx(double a, double b, double epsilon = EPSILON); ///< Checks if a~b (i.e., |a-b| < epsilon)
	bool approx(const Vec3& a, const Vec3& b, double epsilon = EPSILON);
	
}

#include "functions.inc"

#endif
