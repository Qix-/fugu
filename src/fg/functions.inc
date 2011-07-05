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
    // 1D Catmull-Rom spline intermpolation
    template <class Real>
    Real spline(Real x, int nknots, Real * knot)
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


} // fg

