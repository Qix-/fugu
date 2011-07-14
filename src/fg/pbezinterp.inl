/**
 * \file
 * \brief Defines fg::spline::PBezInterp
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
        PBezInterp<T>::PBezInterp( const std::vector<T> &controlPoints )
            : Interpolator<T>()
        {
            setControlPoints( controlPoints );
        }

        template<class T>
        PBezInterp<T>::PBezInterp( const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients )
            : Interpolator<T>()
        {
            setControlPoints( controlPoints, gradients );
        }

        template<class T>
        PBezInterp<T>::PBezInterp()
            : Interpolator<T>()
        {
            mNumControlPoints = 0;
        }

        template<class T>
        PBezInterp<T>::PBezInterp( const PBezInterp<T> &other )
        {
            *this = other;
        }

        template<class T>
        PBezInterp<T>& PBezInterp<T>::operator=( const PBezInterp<T> &other )
        {
            Interpolator<T>::operator=( other );
            mNumControlPoints = other.mNumControlPoints;
            mFirstControlPoint = other.mFirstControlPoint;
            mSegInterpolators = other.mSegInterpolators;
            return *this;
        }

        template<class T>
        void PBezInterp<T>::setControlPoints( const std::vector<T> &controlPoints )
        {
            std::vector< std::pair<T, T> > gradients( controlPoints.size() - 1, std::pair<T, T>( Vec3( 0., 0., 0. ), Vec3( 0., 0., 0. ) ) );
            setControlPoints( controlPoints, gradients );
        }

        template<class T>
        void PBezInterp<T>::smoothGradients()
        {
        }

        template<class T>
        void PBezInterp<T>::setControlPoints( const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients )
        {
            deleteData();
            mNumControlPoints = controlPoints.size();
            std::vector<T> cp;

            for( int i = 0; i < controlPoints.size() - 1; ++i )
            {
                cp.push_back( controlPoints[i] );
                cp.push_back( controlPoints[i] + gradients[i].first );
                cp.push_back( controlPoints[i + 1] - gradients[i].second );
                cp.push_back( controlPoints[i + 1] );
                mSegInterpolators.push_back( BezInterp<T>( cp ) );
                cp.clear();
            }
        }

        template <class T>
        void PBezInterp<T>::appendControlPoint( const T &controlPoint )
        {
            std::pair<T, T> grad;
            appendControlPoint( controlPoint, grad );
        }

        template <class T>
        void PBezInterp<T>::appendControlPoint( const T &controlPoint, const std::pair<T, T> &g )
        {
            if( getNumSegments() > 0 ) {
                std::vector<T> cp( 4 );
                cp[0] = getControlPoint( getNumSegments() );
                cp[1] = cp[0] + g.first;
                cp[2] = controlPoint - g.second;
                cp[3] = controlPoint;
                mSegInterpolators.push_back( BezInterp<T>( cp ) );
            } else if( getNumControlPoints() == 0 ) {
                mFirstControlPoint = controlPoint;
            } else {
                std::vector<T> cp( 4 );
                cp[0] = mFirstControlPoint;
                cp[1] = cp[0] + g.first;
                cp[2] = controlPoint - g.second;
                cp[3] = controlPoint;
                mSegInterpolators.push_back( BezInterp<T>( cp ) );
            }

            ++mNumControlPoints;
        }

        template<class T>
        void PBezInterp<T>::setControlPoint( int index, const T &cp, const std::pair<T, T> &grad )
        {
        }

        template<class T>
        void PBezInterp<T>::setControlPoint( int index, const T &cp )
        {
        }

        template<class T>
        PBezInterp<T>::~PBezInterp( )
        {
            deleteData();
        }

        template < class T >
        void PBezInterp < T >::getDomain( double &min, double &max ) const
        {
            min = 0.f;
            max = ( double ) getNumSegments();
        }

        template< class T >
        void PBezInterp<T>::deleteData( )
        {
            mSegInterpolators.clear();
            mNumControlPoints = 0;
            Interpolator<T>::deleteData( );
        }

        template< class T >
        int PBezInterp<T>::getNumControlPoints() const
        {
            return mNumControlPoints;
        }

        template< class T >
        int PBezInterp<T>::getNumSegments() const
        {
            return getNumControlPoints() - 1;
        }

        template<class T>
        T PBezInterp<T>::getPosition( double t ) const
        {
            int seg = getSegment( t );
            return mSegInterpolators[seg].getPosition( t - ( double ) seg );
        }

        template<class T>
        T PBezInterp<T>::getDerivative( double t ) const
        {
            int seg = getSegment( t );
            return mSegInterpolators[seg].getDerivative( t - ( double ) seg );
        }

        template<class T>
        T PBezInterp<T>::getSecondDerivative( double t ) const
        {
            int seg = getSegment( t );
            return mSegInterpolators[seg].getSecondDerivative( t - ( double ) seg );
        }

        template < class T >
        int PBezInterp < T >::getSegment( double t ) const
        {
            int ti = ( int ) t;
            ti = clamp<int>( ti, 0, getNumSegments() - 1 );
            return ti;
        }

        template <class T>
        std::vector< T > PBezInterp<T>::getSegmentControlPoints( int seg ) const
        {
            seg = clamp<int>( seg, 0, getNumSegments() - 1 );
            return mSegInterpolators[seg].getControlPoints();
        }

        template <class T>
        T PBezInterp<T>::getControlPoint( int i ) const
        {
            i = clamp( i, 0, getNumControlPoints() - 1 );

            if( i = getNumSegments() )
                return getSegmentControlPoints( i - 1 )[4];

            return getSegmentControlPoints( i )[0];
        }

        template <class T>
        std::vector<T> PBezInterp<T>::getControlPoints() const
        {
            std::vector<T> cp;

            for( int i = 0; i < getNumSegments(); ++i ) {
                cp.push_back( getSegmentControlPoints( i )[0] );
            }

            cp.push_back( getSegmentControlPoints( getNumSegments() - 1 )[3] );
            return cp;
        }


    }
}
