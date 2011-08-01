/**
 * \file
 * \brief Defines fg::spline::CRInterp
 * \author james
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

namespace fg {
    namespace spline {

        template<class T>
        CRInterp<T>::CRInterp( const std::vector<T> &controlPoints )
            : Interpolator<T>()
        {
            setControlPoints( controlPoints );
        }

        template<class T>
        CRInterp<T>::CRInterp()
            : Interpolator<T>()
        {
        }

        template<class T>
        CRInterp<T>::CRInterp( const CRInterp<T> &other )
            : Interpolator<T>( other )
        {
            setControlPoints( other.getControlPoints() );
        }

        template<class T>
        CRInterp<T>& CRInterp<T>::operator=( const CRInterp<T> &other )
        {
            Interpolator<T>::operator=( other );
            setControlPoints( other.getControlPoints() );
            return *this;
        }

        template<class T>
        void CRInterp<T>::setControlPoints( const std::vector<T> &controlPoints )
        {
            Interpolator<T>::setControlPoints( controlPoints );
        }

        template<class T>
        void CRInterp<T>::appendControlPoint( const T &cp )
        {
            Interpolator<T>::appendControlPoint( cp );
        }

        template<class T>
        CRInterp<T>::~CRInterp()
        {
        }

		template<class T>
		void CRInterp<T>::getLocalControlPoints( double t, double &tL, T &P0, T &P1, T &P2, T &P3 ) const
		{
        	/* Find the appropriate segment. */
        	int seg = clamp<int>((int) t, 0., getNumSegments() - 1);

			tL = clamp<double>(t - seg, 0., 1.);

			/* Get the required control points */
			P1 = Interpolator<T>::getControlPoint( seg );
			P2 = Interpolator<T>::getControlPoint( seg + 1 );
			if( seg == 0 ) {
				P0 = P1 * 2. - P2;
			} else {
				P0 = Interpolator<T>::getControlPoint( seg - 1 );
			}

			if( seg == getNumSegments() - 1 ) {
				P3 = P2 * 2. - P1;
			} else {
				P3 = Interpolator<T>::getControlPoint( (seg + 2) % Interpolator<T>::getNumControlPoints() );
			}
		}

		template<class T>
		void CRInterp<T>::getCoeff( T &P0, T &P1, T &P2, T &P3, T *A, T *B, T *C, T *D ) const
		{
			if( A ) {
				*A = P1 * 2.;
			}

			if( B ) {
				*B = P2 - P0;
			}

			if( C ) {
				*C = P0 * 2. - P1 * 5. + P2 * 4. - P3;
			}

			if( D ) {
				*D = P0 * -1. + P1 * 3. + P2 * -3. + P3;
			}
		}

        template<class T>
        T CRInterp<T>::getPosition( double t ) const
        {
			/* Get the local t and control points */
			double tL;

 			T P0, P1, P2, P3;

			getLocalControlPoints( t, tL, P0, P1, P2, P3 );

			/* Get the coeffs */
			T A, B, C, D;

			getCoeff( P0, P1, P2, P3, &A, &B, &C, &D );

			/* Do the math */
			double tL2 = tL * tL;

//			std::cout << "t = " << t << ", tL = " << tL << std::endl;
//			std::cout << "P0 = " << P0 << ", P1 = " << P1 << " P2 = " << P2 << ", P3 = " << P3 << "\n";
//			std::cout << "pos = " << (A + B * tL + C * tL2 + D * tL * tL2) * .5 << "\n\n";

			return (A + B * tL + C * tL2 + D * tL * tL2) * .5;
        }

        template<class T>
        T CRInterp<T>::getDerivative( double t ) const
        {
			/* Get the local t and control points */
			double tL;

 			T P0, P1, P2, P3;

			getLocalControlPoints( t, tL, P0, P1, P2, P3 );

			/* Get the coeffs */
			T B, C, D;

			getCoeff( P0, P1, P2, P3, 0, &B, &C, &D );

			/* Do the math */
			return ( B + C * 2. * tL + D * 3. * tL * tL) * .5;
        }

        template<class T>
        T CRInterp<T>::getSecondDerivative( double t ) const
        {
			/* Get the local t and control points */
			double tL;

 			T P0, P1, P2, P3;

			getLocalControlPoints( t, tL, P0, P1, P2, P3 );

			/* Get the coeffs */
			T C, D;

			getCoeff( P0, P1, P2, P3, 0, 0, &C, &D );

			/* Do the math */
			return ( C * 2. + D * 6. * tL) * .5;
        }

        template < class T >
        void CRInterp< T >::getDomain( double &min, double &max ) const
        {
            min = 0.f;
            max = ( double ) getNumSegments();
        }

        template<class T>
        int CRInterp<T>::getNumSegments() const
        {
			if (Interpolator<T>::mOpen)
            	return Interpolator<T>::getNumControlPoints() - 1;
			else
            	return Interpolator<T>::getNumControlPoints();
        }

		template<class T>
		Interpolator<T> * CRInterp<T>::scale( double scale ) const
		{
			std::vector<T> cp = Interpolator<T>::getControlPoints();

			for( int i = 0; i < cp.size(); ++i)
				cp[i] = cp[i] * scale;

			return new CRInterp<T>( cp );
		}
    }
}
