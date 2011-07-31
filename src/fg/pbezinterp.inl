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
			: Interpolator<T>()
        {
            *this = other;
        }

        template<class T>
        PBezInterp<T>& PBezInterp<T>::operator=( const PBezInterp<T> &other )
        {
            Interpolator<T>::operator=( other );

			setControlPoints( other.getControlPoints(), other.getGradients() );
            return *this;
        }

        template<class T>
        void PBezInterp<T>::setControlPoints( const std::vector<T> &controlPoints )
        {
            std::vector< std::pair<T, T> > gradients( controlPoints.size() );
            setControlPoints( controlPoints, gradients );
			smoothGradients();
        }

        template<class T>
        void PBezInterp<T>::smoothGradients()
        {
			mGradients.front().first = getControlPoint(0) * 0.;
			for( int i = 1; i < mGradients.size() - 1; ++i )
			{
				mGradients[i].first = (getControlPoint((i+1)%mGradients.size()) - getControlPoint((i-1)%mGradients.size())) * 0.2;
				mGradients[i].second = mGradients[i].first;
			}
			mGradients.back().second = getControlPoint(0) * 0.;
        }

        template<class T>
        void PBezInterp<T>::setControlPoints( const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients )
        {
            deleteData();
            mNumControlPoints = controlPoints.size();
            std::vector<T> cp;

			mGradients = gradients;

            for( int i = 0; i < controlPoints.size() - 1; ++i )
            {
                cp.push_back( controlPoints[i] );
                cp.push_back( controlPoints[i] + gradients[i].second );
                cp.push_back( controlPoints[i + 1] - gradients[i+1].first );
                cp.push_back( controlPoints[i + 1] );
                mSegInterpolators.push_back( BezInterp<T>( cp ) );
                cp.clear();
            }
            cp.push_back( controlPoints.back() );
            cp.push_back( controlPoints.back() + gradients.back().second );
            cp.push_back( controlPoints.front() - gradients.front().first );
            cp.push_back( controlPoints.front() );

//			std::cout << cp[0] << std::endl;
//			std::cout << cp[1] << std::endl;
//			std::cout << cp[2] << std::endl;
//			std::cout << cp[3] << std::endl;
//			std::cout << "\n\n";

			mClosingInterp = BezInterp<T>( cp );
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
			// Normal case
            if( getNumControlPoints() > 1 ) {
                std::vector<T> cp( 4 );
                cp[0] = getControlPoint( getNumControlPoints() - 1 );
                cp[1] = cp[0] + mGradients.back().second;
                cp[2] = controlPoint - g.first;
                cp[3] = controlPoint;
                mSegInterpolators.push_back( BezInterp<T>( cp ) );

				cp.clear();
				cp.push_back( controlPoint );
				cp.push_back( controlPoint + g.second );
                cp.push_back( getControlPoint( 0 ) - mGradients.front().first );
                cp.push_back( getControlPoint( 0 ) );
				mClosingInterp = BezInterp<T>( cp );
				
				mGradients.push_back( g );
            } else if( getNumControlPoints() == 0 ) { // First point
                mFirstControlPoint = controlPoint;
				mGradients.push_back( g );
            } else { // Second point
                std::vector<T> cp( 4 );
                cp[0] = mFirstControlPoint;
                cp[1] = cp[0] + mGradients.back().second;
                cp[2] = controlPoint - g.first;
                cp[3] = controlPoint;
                mSegInterpolators.push_back( BezInterp<T>( cp ) );

				cp.clear();
				cp.push_back( controlPoint );
				cp.push_back( controlPoint + g.second );
                cp.push_back( getControlPoint( 0 ) - mGradients.front().first );
                cp.push_back( getControlPoint( 0 ) );
				mClosingInterp = BezInterp<T>( cp );

				mGradients.push_back( g );
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
			if (Interpolator<T>::mOpen)
            	return getNumControlPoints() - 1;
			else
            	return getNumControlPoints();
        }

        template<class T>
        T PBezInterp<T>::getPosition( double t ) const
        {
			int seg = getSegment(t);
            return getSegmentInterpolator(seg).getPosition( t - ( double ) seg );
        }

        template<class T>
        T PBezInterp<T>::getDerivative( double t ) const
        {
			int seg = getSegment(t);
            return getSegmentInterpolator(seg).getDerivative( t - ( double ) seg );
        }

        template<class T>
        T PBezInterp<T>::getSecondDerivative( double t ) const
        {
			int seg = getSegment(t);
            return getSegmentInterpolator(seg).getSecondDerivative( t - ( double ) seg );
        }

        template<class T>
        int PBezInterp<T>::getSegment( double t ) const
        {
            int ti = ( int ) t;
           	ti = clamp<int>( ti, 0, getNumSegments() - 1 );

            return ti;
        }

        template <class T>
        std::vector< T > PBezInterp<T>::getSegmentControlPoints( int seg ) const
        {
           	seg = clamp<int>( seg, 0, getNumSegments() - 1 );

			if( seg < getNumControlPoints() - 1 )
            	return mSegInterpolators[seg].getControlPoints();
			else
			{
				return mClosingInterp.getControlPoints();
			}
        }

        template <class T>
		std::vector< std::pair<T,T> > PBezInterp<T>::getGradients( ) const
		{
			return mGradients;
		}

        template <class T>
        T PBezInterp<T>::getControlPoint( int i ) const
        {
            i = clamp( i, 0, getNumControlPoints() - 1 );

            if( i == getNumControlPoints() - 1 )
                return getSegmentControlPoints( i - 1 )[3];

            return getSegmentControlPoints( i )[0];
        }

        template <class T>
        std::pair<T, T> PBezInterp<T>::getGradient( int i ) const
        {
            i = clamp( i, 0, getNumControlPoints() - 1 );

            return mGradients[i];
        }


        template <class T>
        std::vector<T> PBezInterp<T>::getControlPoints() const
        {
            std::vector<T> cp;

			if( getNumControlPoints() == 0 )
				cp.push_back( mFirstControlPoint );
			else
			{
            	for( int i = 0; i < getNumControlPoints() - 1; ++i ) {
                	cp.push_back( getSegmentControlPoints( i )[0] );
            	}
				cp.push_back( getSegmentControlPoints( getNumControlPoints() - 2 )[3] );
			}

            return cp;
        }

		template<class T>
		const BezInterp<T> PBezInterp<T>::getSegmentInterpolator( int seg ) const
		{
			if (seg == getNumControlPoints() - 1 )
				return mClosingInterp;
			else
				return mSegInterpolators[seg];
		}

		template<class T>
		Interpolator<T> * PBezInterp<T>::scale( double s ) const
		{
			std::vector<T> cp = getControlPoints();
			std::vector< std::pair<T,T> > grad = getGradients();

			for(int i = 0; i < cp.size(); ++i)
			{
				cp[i] = cp[i] * s;
				grad[i].first = grad[i].first * s;
				grad[i].second = grad[i].second * s;
			}

			return new PBezInterp<T>(cp, grad);
		}
    }
}
