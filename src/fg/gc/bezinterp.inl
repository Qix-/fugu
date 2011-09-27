/**
 * \file
 * \brief Defines fg::gc::BezInterp
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
    namespace gc {

        template<class T>
        BezInterp<T>::BezInterp( const std::vector<T> &controlPoints )
            : Interpolator<T>()
            , mChoose( NULL )
        {
            setControlPoints( controlPoints );
        }

        template<class T>
        BezInterp<T>::BezInterp()
            : Interpolator<T>()
            , mChoose( NULL )
        {
        }

        template<class T>
        BezInterp<T>::BezInterp( const BezInterp<T> &other )
            : Interpolator<T>( other )
            , mChoose( NULL )
        {
            setControlPoints( other.getControlPoints() );
        }

        template<class T>
        BezInterp<T>& BezInterp<T>::operator=( const BezInterp<T> &other )
        {
            Interpolator<T>::operator=( other );
            setControlPoints( other.getControlPoints() );
            return *this;
        }

        template<class T>
        void BezInterp<T>::allocateChoose()
        {
            int i, j;

            if( Interpolator<T>::getNumControlPoints() < 2 )
                return;

            mChoose = Interpolator<double>::allocate2dArray( Interpolator<T>::getNumControlPoints(), Interpolator<T>::getNumControlPoints() );
            mChoose[0][0] = ( double )1;
            mChoose[1][0] = ( double )1;
            mChoose[1][1] = ( double )1;

            for( i = 2; i <= getDegree(); ++i )
            {
                mChoose[i][0] = ( double )1;
                mChoose[i][i] = ( double )1;

                for( j = 1; j < i; ++j )
                {
                    mChoose[i][j] = mChoose[i - 1][j - 1] + mChoose[i - 1][j];
                }
            }
        }

        template<class T>
        void BezInterp<T>::deleteChoose()
        {
            if( mChoose )
                Interpolator<double>::delete2dArray( mChoose );

            mChoose = NULL;
        }

        template<class T>
        void BezInterp<T>::setControlPoints( const std::vector<T> &controlPoints )
        {
            deleteData();
            Interpolator<T>::setControlPoints( controlPoints );
            int i;

            for( i = 0; i < Interpolator<T>::getNumControlPoints() - 1; ++i )
            {
                mDer1ControlPoint.push_back( Interpolator<T>::mControlPoints[i + 1] - Interpolator<T>::mControlPoints[i] );
            }

            for( i = 0; i < Interpolator<T>::getNumControlPoints() - 2; ++i )
            {
                mDer2ControlPoint.push_back( mDer1ControlPoint[i + 1] - mDer1ControlPoint[i] );
            }

            allocateChoose();
        }

        template<class T>
        void BezInterp<T>::appendControlPoint( const T &cp )
        {
            deleteChoose();
            Interpolator<T>::appendControlPoint( cp );
            mDer1ControlPoint.push_back( Interpolator<T>::mControlPoints[Interpolator<T>::getNumControlPoints()] - Interpolator<T>::mControlPoints[getDegree()] );
            mDer2ControlPoint.push_back( mDer1ControlPoint[getDegree()] - mDer1ControlPoint[getDegree() - 1] );
            allocateChoose();
        }

        template<class T>
        BezInterp<T>::~BezInterp()
        {
            deleteData();
        }

        template<class T>
        void BezInterp<T>::deleteData()
        {
            mDer1ControlPoint.clear();
            mDer2ControlPoint.clear();
            Interpolator<T>::deleteData();
            deleteChoose();
        }

        template<class T>
        int BezInterp<T>::getDegree() const
        {
            return Interpolator<T>::getNumControlPoints() - 1;
        }

        template<class T>
        T BezInterp<T>::getPosition( double t ) const
        {
            t = clamp<double>( t, 0., 1. );
            double oneMinusT = ( double )1 - t;
            double powT = t;
            T result = Interpolator<T>::mControlPoints[0] * oneMinusT;

            for( int i = 1; i < getDegree(); ++i )
            {
                double coeff = mChoose[getDegree()][i] * powT;
                result = ( result + Interpolator<T>::mControlPoints[i] * coeff ) * oneMinusT;
                powT *= t;
            }

            result += Interpolator<T>::mControlPoints[getDegree()] * powT;
            return result;
        }

        template<class T>
        T BezInterp<T>::getDerivative( double t ) const
        {
            t = clamp<double>( t, 0., 1. );
            double oneMinusT = ( double )1 - t;
            double powT = t;
            T result = mDer1ControlPoint[0] * oneMinusT;
            int degreeM1 = Interpolator<T>::getNumControlPoints() - 2;

            for( int i = 1; i < degreeM1; ++i )
            {
                double coeff = mChoose[degreeM1][i] * powT;
                result = ( result + mDer1ControlPoint[i] * coeff ) * oneMinusT;
                powT *= t;
            }

            result += mDer1ControlPoint[degreeM1] * powT;
            result *= double( Interpolator<T>::getNumControlPoints() - 1 );
            return result;
        }

        template<class T>
        T BezInterp<T>::getSecondDerivative( double t ) const
        {
            t = clamp<double>( t, 0., 1. );
            double oneMinusT = ( double )1 - t;
            double powT = t;
            T result = mDer2ControlPoint[0] * oneMinusT;
            int degreeM2 = Interpolator<T>::getNumControlPoints() - 3;

            for( int i = 1; i < degreeM2; ++i )
            {
                double coeff = mChoose[degreeM2][i] * powT;
                result = ( result + mDer2ControlPoint[i] * coeff ) * oneMinusT;
                powT *= t;
            }

            result += mDer2ControlPoint[degreeM2] * powT;
            result *= double( getDegree() * ( getDegree() - 1 ) );
            return result;
        }

        template<class T>
        int BezInterp<T>::getNumSegments() const
        {
            return 1;
        }

		template<class T>
		Interpolator<T> * BezInterp<T>::scale( double scale ) const
		{
			std::vector<T> cp = Interpolator<T>::getControlPoints();

			for( int i = 0; i < cp.size(); ++i)
				cp[i] = cp[i] * scale;

			return new BezInterp<T>( cp );
		}
    }
}
