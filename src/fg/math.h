//
//  math.h
//  FG
//
//  Created by Jon McCormack on 5/07/11.
//  Copyright 2011 Monash University. All rights reserved.
//

#ifndef MATH_H
#define MATH_H

// standard math functions in <cmath> include overloaded functions for floats,
// doubles, etc. so not necessary to redefine them here
//
#include <cmath>

namespace fg
{
    
    template <typename Real>  
	Real InvSqrt (Real fValue) { return 1.0/std::sqrt(fValue); }
    
    template <typename Real>  
	inline Real Sqr (Real fValue) { return fValue * fValue; }
    
    template <typename Real>  
	Real Sign (Real fValue);
    
	// Fast, approximate routines
    // The input must be in [0,pi/2]
    template <typename Real>  Real FastSin0 (Real fAngle);
    template <typename Real>  Real FastSin1 (Real fAngle);
    
    // The input must be in [0,pi/2]
    template <typename Real>  Real FastCos0 (Real fAngle);
    template <typename Real>  Real FastCos1 (Real fAngle);
    
    // The input must be in [0,pi/4].
    template <typename Real>  Real FastTan0 (Real fAngle);
    template <typename Real>  Real FastTan1 (Real fAngle);
    
    // The input must be in [0,1].
    template <typename Real>  Real FastInvSin0 (Real fValue);
    template <typename Real>  Real FastInvSin1 (Real fValue);
    
    // The input must be in [0,1].
    template <typename Real>  Real FastInvCos0 (Real fValue);
    template <typename Real>  Real FastInvCos1 (Real fValue);
    
    // The input must be in [-1,1]. 
    // max error invtan0 = 1.2e-05, speed up = 2.8
    // max error invtan1 = 2.3e-08, speed up = 1.8
    template <typename Real>  Real FastInvTan0 (Real fValue);
    template <typename Real>  Real FastInvTan1 (Real fValue);
    
    template <typename Real>  Real FastInvSqrt (Real fValue);
    
    // common constants
	const double LOG05 = -0.693147180559945; 
	
#include "math.inc"
    
} // fg

#endif // MATH_H
