#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

/**
 * Useful mathematical and geometric functions.
 */

#include <cfloat>
#include <cmath>

#include "fg/vec3.h"
#include "fg/mat4.h"

namespace fg {

	// common constants
    const double EPSILON = DBL_EPSILON; // A small double..
	const double LOG05 = -0.693147180559945;

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
	Real catmullSpline(Real x, int knots, Real * knot);
    
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


	template <typename Real>
	Real invSqrt (Real fValue) { return 1.0/std::sqrt(fValue); }

	template <typename Real>
	inline Real sqr (Real fValue) { return fValue * fValue; }

	template <typename Real>
	Real sign (Real fValue);

	// Fast, approximate routines
	// The input must be in [0,pi/2]
	template <typename Real>  Real fastSin0 (Real fAngle);
	template <typename Real>  Real fastSin1 (Real fAngle);

	// The input must be in [0,pi/2]
	template <typename Real>  Real fastCos0 (Real fAngle);
	template <typename Real>  Real fastCos1 (Real fAngle);

	// The input must be in [0,pi/4].
	template <typename Real>  Real fastTan0 (Real fAngle);
	template <typename Real>  Real fastTan1 (Real fAngle);

	// The input must be in [0,1].
	template <typename Real>  Real fastInvSin0 (Real fValue);
	template <typename Real>  Real fastInvSin1 (Real fValue);

	// The input must be in [0,1].
	template <typename Real>  Real fastInvCos0 (Real fValue);
	template <typename Real>  Real fastInvCos1 (Real fValue);

	// The input must be in [-1,1].
	// max error invtan0 = 1.2e-05, speed up = 2.8
	// max error invtan1 = 2.3e-08, speed up = 1.8
	template <typename Real>  Real fastInvTan0 (Real fValue);
	template <typename Real>  Real fastInvTan1 (Real fValue);

	template <typename Real>  Real fastInvSqrt (Real fValue);

    /*
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
