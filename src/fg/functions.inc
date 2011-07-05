namespace fg {

	template <typename T> 
    T min(const T& a, const T& b){
		return a<b?a:b; 
	}
	
	template <typename T> 
    T lerp(const T& a, const T& b, T t){
		return (b-a)*t + a;
	}

    /// mix
    //
    // Mixes two values, "a" and "b" by amount "mixValue"
    // Requires operator* for a and b
    // mixValue should lie between 0 and 1
    //
    template <class T, class Real>
    inline T mix(const T& a, const T& b, Real mixValue)
    {
        // assert(mixValue >= 0.0 && mixValue <= 1.0f);
        return a * (static_cast<Real>(1) - mixValue) + b * mixValue;
    }
    
    /// clamp
    //  clamps x between a and b
    //
    template <class T>
    inline T clamp(const T& x, const T& a, const T& b)
    {
        return std::min( std::max(x, a), b );
    }
    
    /// step
    //
    // Returns 0 when x < a, 1 otherwise
    template <class T, typename Real>
    inline Real step(const T& a, const T& x)
    {
        return static_cast<Real>(x >= a);
    }
    
    // pulse
    //
    // combination of 2 steps: returns 1 if x is between a and b
    //
    template <class T, typename Real>
    inline Real pulse(const T& a, const T&b, const T& x)
    {
        return step<T,Real>(a, x) - step<T,Real>(b, x);
    }
    
    // smoothstep
    //
    // smooth interpolation using Horner's rule
    template <typename Real>
    Real smoothstep(Real a, Real b, Real x)
    {
        if (x < a) return static_cast<Real>(0);
        if (x >= b) return static_cast<Real>(1);
        
        x = (x - a)/(b - a); 		// normalise to [0:1]
        return (x * x * (3 - 2*x));
    }
    
    // spline
    //
    // 1D Catmull-Rom spline interpolation
    template <class Real>
    Real catmullSpline(Real x, int nknots, Real * knot)
    {
        /* Coefficients of basis matrix. */
        const float CR00 = -0.5;	const float CR01 = 1.5;
        const float CR02 = -1.5;	const float CR03 = 0.5;
        const float CR10 = 1.0;		const float CR11 = -2.5;
        const float CR12 = 2.0;		const float CR13 = -0.5;
        const float CR20 = -0.5;	const float CR21 = 0.0;
        const float CR22 = 0.5;		const float CR23 = 0.0;
        const float CR30 = 0.0;		const float CR31 = 1.0;
        const float CR32 = 0.0;		const float CR33 = 0.0;
        
        int span;
        int nspans = nknots - 3;
        Real c0, c1, c2, c3;	/* coefficients of the cubic.*/
        
        assert(nspans > 0);
        
        /* Find the appropriate 4-point span of the spline. */
        x = clamp(x, 0.0, 1.0) * nspans;
        span = (int) x;
        if (span >= nknots - 3)
            span = nknots - 3;
        x -= span;
        knot += span;
        
        /* Evaluate the span cubic at x using Horner's rule. */
        c3 = CR00*knot[0] + CR01*knot[1]
        + CR02*knot[2] + CR03*knot[3];
        c2 = CR10*knot[0] + CR11*knot[1]
        + CR12*knot[2] + CR13*knot[3];
        c1 = CR20*knot[0] + CR21*knot[1]
        + CR22*knot[2] + CR23*knot[3];
        c0 = CR30*knot[0] + CR31*knot[1]
        + CR32*knot[2] + CR33*knot[3];
        
        return ((c3*x + c2)*x + c1)*x + c0;
    }
    
    /// Bias
    // 1D bias function
    //
    template< class Real >
    inline Real Bias(Real b, Real x)
    {
        return std::pow( x, std::log(b)/static_cast<Real>(LOG05) );
    }
    
    /// FastBias
    //  faster but slightly less accurate than Bias
    //
    template< class Real >
    inline Real fastBias(Real b, Real x)
    {
        // faster bias function (from Schlick), but not as accurate
        return (x / ((1/b - 2)*(1 - x) + 1));
    }
    
    /// Gain
    // Gain Control
    //
    template< class Real >
    inline Real Gain(Real g, Real x)
    {
        if (x < 0.5)
            return Bias(1-g, 2*x)/2;
        else
            return 1.0 - Bias(1-g, 2- 2*x)/2;
    }
    
    /// FastGain
    //  faster but slightly less accurate than Gain
    //
    template< class Real >
    inline Real fastGain(Real g, Real x) {
        // faster gain function (from Schlick), but not as accurate
        if (x < 0.5)
            return x / ((1/g - 2)*(1 - 2*x) + 1);
        else 
        {
            Real ta = (1/g - 2);
            Real tb = (1 - 2*x);
            return (ta * tb - x) / (ta * tb - 1);
        }
    }
    
    /// gammaCorrect
    // standard gamma correction function
    //
    template< class Real >
    inline Real gammaCorrect(Real gamma, Real x)
    {
        return std::pow(x, static_cast<Real>(1.0)/gamma);
    }

    //
    //  math.inc
    //  FG
    //
    //  Created by Jon McCormack on 5/07/11.
    //  Copyright 2011 Monash University. All rights reserved.
    //

    //----------------------------------------------------------------------------
    template <class Real>
    Real sign (Real fValue)
    {
        if ( fValue > (Real)0.0 )
            return (Real)1.0;

        if ( fValue < (Real)0.0 )
            return -(Real)1.0;

        return (Real)0.0;
    }

    //----------------------------------------------------------------------------
    template <class Real>
    Real fastSin0 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = (Real)7.61e-03;
        fResult *= fASqr;
        fResult -= (Real)1.6605e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        fResult *= fAngle;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    inline Real fastSin1 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = -(Real)2.39e-08;
        fResult *= fASqr;
        fResult += (Real)2.7526e-06;
        fResult *= fASqr;
        fResult -= (Real)1.98409e-04;
        fResult *= fASqr;
        fResult += (Real)8.3333315e-03;
        fResult *= fASqr;
        fResult -= (Real)1.666666664e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        fResult *= fAngle;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastCos0 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = (Real)3.705e-02;
        fResult *= fASqr;
        fResult -= (Real)4.967e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastCos1 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = -(Real)2.605e-07;
        fResult *= fASqr;
        fResult += (Real)2.47609e-05;
        fResult *= fASqr;
        fResult -= (Real)1.3888397e-03;
        fResult *= fASqr;
        fResult += (Real)4.16666418e-02;
        fResult *= fASqr;
        fResult -= (Real)4.999999963e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastTan0 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = (Real)2.033e-01;
        fResult *= fASqr;
        fResult += (Real)3.1755e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        fResult *= fAngle;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastTan1 (Real fAngle)
    {
        Real fASqr = fAngle*fAngle;
        Real fResult = (Real)9.5168091e-03;
        fResult *= fASqr;
        fResult += (Real)2.900525e-03;
        fResult *= fASqr;
        fResult += (Real)2.45650893e-02;
        fResult *= fASqr;
        fResult += (Real)5.33740603e-02;
        fResult *= fASqr;
        fResult += (Real)1.333923995e-01;
        fResult *= fASqr;
        fResult += (Real)3.333314036e-01;
        fResult *= fASqr;
        fResult += (Real)1.0;
        fResult *= fAngle;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvSin0 (Real fValue)
    {
        Real fRoot = std::sqrt(1.0-fValue);
        Real fResult = -(Real)0.0187293;
        fResult *= fValue;
        fResult += (Real)0.0742610;
        fResult *= fValue;
        fResult -= (Real)0.2121144;
        fResult *= fValue;
        fResult += (Real)1.5707288;
        fResult = M_PI_2 - fRoot*fResult;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvSin1 (Real fValue)
    {
        Real fRoot = std::sqrt(std::fabs(1.0-fValue));
        Real fResult = -(Real)0.0012624911;
        fResult *= fValue;
        fResult += (Real)0.0066700901;
        fResult *= fValue;
        fResult -= (Real)0.0170881256;
        fResult *= fValue;
        fResult += (Real)0.0308918810;
        fResult *= fValue;
        fResult -= (Real)0.0501743046;
        fResult *= fValue;
        fResult += (Real)0.0889789874;
        fResult *= fValue;
        fResult -= (Real)0.2145988016;
        fResult *= fValue;
        fResult += (Real)1.5707963050;
        fResult = M_PI_2 - fRoot*fResult;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvCos0 (Real fValue)
    {
        Real fRoot = std::sqrt(1.0-fValue);
        Real fResult = -(Real)0.0187293;
        fResult *= fValue;
        fResult += (Real)0.0742610;
        fResult *= fValue;
        fResult -= (Real)0.2121144;
        fResult *= fValue;
        fResult += (Real)1.5707288;
        fResult *= fRoot;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvCos1 (Real fValue)
    {
        Real fRoot = std::sqrt(std::fabs(1.0-fValue));
        Real fResult = -(Real)0.0012624911;
        fResult *= fValue;
        fResult += (Real)0.0066700901;
        fResult *= fValue;
        fResult -= (Real)0.0170881256;
        fResult *= fValue;
        fResult += (Real)0.0308918810;
        fResult *= fValue;
        fResult -= (Real)0.0501743046;
        fResult *= fValue;
        fResult += (Real)0.0889789874;
        fResult *= fValue;
        fResult -= (Real)0.2145988016;
        fResult *= fValue;
        fResult += (Real)1.5707963050;
        fResult *= fRoot;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvTan0 (Real fValue)
    {
        Real fVSqr = fValue*fValue;
        Real fResult = (Real)0.0208351;
        fResult *= fVSqr;
        fResult -= (Real)0.085133;
        fResult *= fVSqr;
        fResult += (Real)0.180141;
        fResult *= fVSqr;
        fResult -= (Real)0.3302995;
        fResult *= fVSqr;
        fResult += (Real)0.999866;
        fResult *= fValue;
        return fResult;
    }
    //----------------------------------------------------------------------------
    template <class Real>
    Real fastInvTan1 (Real fValue)
    {
        Real fVSqr = fValue*fValue;
        Real fResult = (Real)0.0028662257;
        fResult *= fVSqr;
        fResult -= (Real)0.0161657367;
        fResult *= fVSqr;
        fResult += (Real)0.0429096138;
        fResult *= fVSqr;
        fResult -= (Real)0.0752896400;
        fResult *= fVSqr;
        fResult += (Real)0.1065626393;
        fResult *= fVSqr;
        fResult -= (Real)0.1420889944;
        fResult *= fVSqr;
        fResult += (Real)0.1999355085;
        fResult *= fVSqr;
        fResult -= (Real)0.3333314528;
        fResult *= fVSqr;
        fResult += (Real)1.0;
        fResult *= fValue;
        return fResult;
    }
    //----------------------------------------------------------------------------


} // fg

