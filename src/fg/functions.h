/**
 * \file
 * \brief Useful mathematical and animation functions
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

#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

#include <cfloat>
#include <cmath>

#include "fg/vec3.h"
#include "fg/mat4.h"

/**
 * \defgroup functions Functions
 * \brief A collection of useful mathematical functions
 */

namespace fg {

	// common constants
    static const double EPSILON = DBL_EPSILON; // A small double..
    static const double LOG05 = -0.693147180559945;
    static const double PI = M_PI;

	/** 
	 * \brief calculate the minimum of two values
	 * \ingroup functions 
	 */
	template <class T> 
    T min(const T& a, const T& b);
    
	/** 
	 * \brief linearly interpolates between two values
	 * \ingroup functions 
	 */
	template <class T, typename Real> 
    T lerp(const T& a, const T& b, Real t);

	/**
	 * \brief spherical interpolation between two (normalised) vectors
	 * \ingroup functions
	 */
	Vec3 slerp(const Vec3& a, const Vec3& b, double t);
    
	/** 
	 * \brief Mixes two values, "a" and "b" by amount "mixValue"
	 * mixValue should lie between 0 and 1
	 *
	 * \ingroup functions 
	 */
    template <class T, typename Real>
	T mix(const T& a, const T& b, Real mixValue);
    
	/** 
	 * \brief clamps x between a and b
	 * \ingroup functions 
	 */
	template <class T>
	T clamp(const T& x, const T& a, const T& b);
    
	/** 
	 * \brief Returns 0 when x < a, 1 otherwise
	 * \ingroup functions 
	 */
	template <class T, typename Real >
	Real step(const T& a, const T& x);
    
	/** 
	 * \brief combination of 2 steps: returns 1 if x is between a and b
	 * \ingroup functions 
	 */
	template <class T, typename Real >
	Real pulse(const T& a, const T&b, const T& x);
    	
	/** 
	 * \brief smooth interpolation using Horner's rule
	 * \ingroup functions 
	 */
	template <typename Real>
	Real smoothstep(Real a, Real b, Real x);
    
	/** 
	 * \brief 1D Catmull-Rom spline interpolation
	 * \ingroup functions 
	 */
	template <class Real>
	Real catmullSpline(Real x, int knots, Real * knot);
    
	/** 
	 * \brief 1D bias function
	 * \ingroup functions 
	 */
	template <class Real>
	Real bias(Real b, Real x);
    
	/** 
	 * \brief faster but slightly less accurate than Bias
	 * \ingroup functions 
	 */
	template <class Real>
	Real fastBias(Real b, Real x);
    
	/** 
	 * \brief Gain Control
	 * \ingroup functions 
	 */
	template <class Real>
	Real gain(Real g, Real x);
    
	/** 
	 * \brief faster but slightly less accurate than Gain
	 * \ingroup functions 
	 */
	template <class Real>
	Real fastGain(Real g, Real x);
    
	/** 
	 * \brief standard gamma correction function
	 * \ingroup functions 
	 */
	template <class Real>
	Real gammaCorrect(Real gamma, Real x);

	/** 
	 * \brief 
	 * \ingroup functions 
	 */
	template <typename Real>
	Real invSqrt (Real fValue) { return 1.0/std::sqrt(fValue); }

	/** 
	 * \brief 
	 * \ingroup functions 
	 */
	template <typename Real>
	inline Real sqr (Real fValue) { return fValue * fValue; }

	/** 
	 * \brief 
	 * \ingroup functions 
	 */
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

    /**
     * noise functions currently use vcg's Perlin noise function
     */
	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);
    
    /* *
     * fracSum and turbulence functions
     */
    double fracSum(double x, double y, double z, int nOctaves, double falloff = 1.0);
    double turbulence(double x, double y, double z, int nOctaves, double falloff = 1.0);

    /**
     * random currently uses the OS rand function (beware!)
     */
	double random(); ///< Returns a random number between 0 and 1 inclusive
	double random(double low, double high); ///< Returns a random number between low and high inclusive
    double randomN(double mean, double var); ///< Returns a random number with Normal(Gaussian) distribution

	bool approx(double a, double b, double epsilon = EPSILON); ///< Checks if a~b (i.e., |a-b| < epsilon)
	bool approx(const Vec3& a, const Vec3& b, double epsilon = EPSILON);
	
}

#include "functions.inl"

#endif
