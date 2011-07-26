/**
 * \file
 * \brief Defines fg::gc::PBezCarrier
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

#include "fg/fg.h"
#include "fg/mat4.h"
#include "fg/pbezcarrier.h"

using namespace std;

namespace fg {
    namespace gc {

        PBezCarrier::PBezCarrier( const vector<Mat4> &refFrames )
            : mInterpolator( NULL )
        {
            setReferenceFrames( refFrames );
        }

        PBezCarrier::PBezCarrier( const std::vector<Mat4> &refFrames, const std::vector< std::pair<double, double> > &stiffness )
            : mInterpolator( NULL )
        {
            setReferenceFrames( refFrames, stiffness );
        }

        PBezCarrier::PBezCarrier( const PBezCarrier &other )
            : mInterpolator( NULL )
        {
            *this = other;
        }

        PBezCarrier::PBezCarrier()
            : mInterpolator( NULL )
        {
        }

        PBezCarrier &PBezCarrier::operator=( const PBezCarrier &other )
        {
            if( other.mInterpolator == NULL )
                this->mInterpolator = NULL;
            else
                this->mInterpolator = new spline::PBezInterpDiv( *( other.mInterpolator ) );

            this->mInflectionPoints = other.mInflectionPoints;
            this->mSegType = other.mSegType;
            this->mRFrames = other.mRFrames;
            return *this;
        }

        void PBezCarrier::setReferenceFrames( const std::vector<Mat4> &rFrames )
        {
            vector< pair<double, double> > stiffness( rFrames.size() - 1 );

            for( int i = 0; i < rFrames.size() - 1; ++i )
            {
                stiffness[i].first = 0.6;
                stiffness[i].second = 0.6;
            }

            setReferenceFrames( rFrames, stiffness );
        }

        void PBezCarrier::setReferenceFrames( const std::vector<Mat4> &refFrames, const std::vector< std::pair<double, double> > &stiffness )
        {
            deleteData();
            vector<Vec3> tmpCp;
            vector< pair<Vec3, Vec3> > tmpGrad;

 			PBezCarrier::stiffnessToGrad( refFrames, stiffness, tmpCp, tmpGrad );

            mInterpolator = new spline::PBezInterpDiv( tmpCp, tmpGrad );
            // Store our data
            mRFrames = refFrames;

            // Find inflection points
            for( int i = 0; i < refFrames.size() - 1; ++i )
            {
                mSegType.push_back( 0 );
                mInflectionPoints.push_back( pair<double, double>( 0., 0. ) );
                updateInflectionPoints( i );
            }
        }

        vector<Mat4> PBezCarrier::getReferenceFrames() const
        {
            return mRFrames;
        }

        PBezCarrier::~PBezCarrier()
        {
            deleteData();
        }

        void PBezCarrier::deleteData()
        {
            if( mInterpolator )
                delete mInterpolator;

            mInflectionPoints.clear();
            mSegType.clear();
            mRFrames.clear();
        }

        void PBezCarrier::updateInflectionPoints( int seg )
        {
            // No such segment
            if( seg > ( signed int )( mInterpolator->getNumSegments() ) || seg < 0 ) {
                return;
            }

            // Check if the line is straight
            vector<Vec3> cp = mInterpolator->getSegmentControlPoints( seg );

            if( collinear( cp ) ) {
                mSegType[seg] = 3;
            } else {
                double ip1, ip2;
                mSegType[seg] = findInflectionPointsVec( cp, &ip1, &ip2 );
                mInflectionPoints[seg] = std::pair<float, float>( ip1, ip2 );
            }
        }

        const spline::PBezInterpDiv * PBezCarrier::getInterpolator() const
        {
            return mInterpolator;
        }

        Quat PBezCarrier::getFrenetFrame( double v ) const
        {
            Vec3 vel = mInterpolator->getDerivative( v );
            Vec3 tangent = vel;
            tangent.normalise();
            Vec3 acc = mInterpolator->getSecondDerivative( v );
            double vDotV = vel.dot( vel );
            double vDotA = vel.dot( acc );
            Vec3 norm = acc * vDotV - vel * vDotA;
            norm.normalise();
            Quat qt = Quat( Vec3( 1., 0., 0. ), tangent );
            //Quat qn = Quat(qt * Vec3(0.,1.,0.), norm);
            return qt;//*qn;
        }

        Quat PBezCarrier::orient( double v ) const
        {
            int vint = ( int ) v;
            vint = clamp<int>( vint, 0, mInterpolator->getNumSegments() - 1 );
            double localV = v - ( double ) vint;
            Quat ff = getFrenetFrame( v );
            /*
                switch (segmentType[vint]) {
                case 0: // No inflection
                    break;
                case 1: // One inflection
                    if (spline::Interpoloator<double>::AlmostEqual2sComplement( localV, inflectionPoints[seg].first, 10000 )) {
                        ff = getFrenetFrame( v - 0.05 );
                    } else if (localU > inflectionPoints[seg].first) {
                        mat.rotate( Vec3f(0.f, 0.f, 1.f), M_PI );
                    }
                    break;
                case 2:
                    if (miscUtils::AlmostEqual2sComplement( localU, inflectionPoints[seg].first, 10000 )) {
                        mat = getFrenetFrame( seg, localU - 0.05 );
                    } else if (localU > inflectionPoints[seg].first) {
                        mat.rotate( Vec3f(0.f, 0.f, 1.f), M_PI );
                    }
                    break;
                    if (miscUtils::AlmostEqual2sComplement( localU, inflectionPoints[seg].second, 10000 )) {
                        mat = getFrenetFrame( seg, localU - 0.05 );
                    } else if (localU > inflectionPoints[seg].second) {
                        mat.rotate( Vec3f(0.f, 0.f, 1.f), M_PI );
                    }
                    break;
                case 3: // Straight
                {
                    mat = Matrix44<float>( norm, binorm, tan ).inverted();
                    break;
                }
                default:
                    break;
                }

                */
            return ff;
        }

        Quat PBezCarrier::orientDer( double v ) const
        {
            return Quat();
        }

        pair<Quat, Quat> PBezCarrier::getSegOrients( int seg ) const
        {
            double min, max;
            min = ( double ) seg;
            max = min + 0.99999;
            min = clamp<double>( min, min + 0.000001, min + 1. );
            Quat q1 = orient( min );
            Quat q2 = orient( max );
            return pair<Quat, Quat>( q1, q2 );
        }
		
		void PBezCarrier::stiffnessToGrad( const std::vector<Mat4> &refFrames, const std::vector< std::pair<double,double> > &stiffness,
								 		   std::vector<Vec3> &pos, std::vector< std::pair<Vec3,Vec3> > &grad )
		{
			pos.clear();
			pos.resize( refFrames.size() );

			grad.clear();
			grad.resize( refFrames.size() );

			Mat4 tmp;
			// first and last gradient for closed curves
			if( refFrames.size() > 1) {
                // Get the control points and headings 
                Vec3 p1 = refFrames.back()  * Vec3( 0., 0., 0. ); // The pos
                Vec3 p2 = refFrames.back()  * Vec3( 0., 0., 1. ) - p1; // The head
                Vec3 p4 = refFrames.front() * Vec3( 0., 0., 0. );
                Vec3 p3 = refFrames.front() * Vec3( 0., 0., 1. ) - p4;
                // Calculate the tangents using coefficients
                float mag = ( p1 - p4 ).Norm();
                p2 = p2 * mag * ( stiffness.back().first );
                p3 = p3 * mag * ( stiffness.front().second );

                grad.back().second = p2;
                grad.front().first = p3;
			}
            for( int i = 0; i < refFrames.size() - 1; ++i )
            {
                // Get the control points and headings PUT THIS INTO MAT4!!
                Vec3 p1 = refFrames[i]   * Vec3( 0., 0., 0. ); // The pos
                Vec3 p2 = refFrames[i]   * Vec3( 0., 0., 1. ) - p1; // The head
                Vec3 p4 = refFrames[i + 1] * Vec3( 0., 0., 0. );
                Vec3 p3 = refFrames[i + 1] * Vec3( 0., 0., 1. ) - p4;

                // Calculate the tangents using coefficients
                float mag = ( p1 - p4 ).Norm();
                p2 = p2 * mag * ( stiffness[i].second );
                p3 = p3 * mag * ( stiffness[i+1].first );
                pos[i] = p1;
                pos[i + 1] = p4;
                grad[i].second = p2;
                grad[i + 1].first = p3;
            }
		}

        int PBezCarrier::findInflectionPointsVec( const vector<Vec3> &cp, double *i1, double *i2 )
        {
            Vec3 a = cp[1] - cp[0];
            Vec3 b = cp[2] - cp[1] - a;
            Vec3 c = cp[3] - cp[2] - a - b * 2;
            // Check for inflection points
            int n = roots( c ^ b, c ^ a, b ^ a, i1, i2 );

            // Check they fall in the correct domain
            if( n == 2 && ( *i2 < 0 || *i2 > 1 ) )
                n = 1;

            if( *i1 < 0 || *i1 > 1 )
            {
                *i1 = *i2;
                --n;
            }

            return n;
        }

        int PBezCarrier::roots( Vec3 a, Vec3 b, Vec3 c, double *r1, double *r2 )
        {
            bool xall, yall, zall;
            // first component
            double rootsx[2];
            int irootsx = roots( a.getX(), b.getX(), c.getX(), &rootsx[0], &rootsx[1] );

            if( irootsx == 0 ) {
                return 0;
            }

            xall = irootsx < 0;
            // second component
            double rootsy[2];
            int irootsy = roots( a.getY(), b.getY(), c.getY(), &rootsy[0], &rootsy[1] );

            if( irootsy == 0 ) {
                return 0;
            }

            yall = irootsy < 0;
            // third component
            double rootsz[2];
            int irootsz = roots( a.getZ(), b.getZ(), c.getZ(), &rootsz[0], &rootsz[1] );

            if( irootsz == 0 ) {
                return 0;
            }

            zall = irootsz < 0;

            // Zero everywhere
            if( xall && yall && zall ) {
                return -1;
            }

            // find common roots
            // intersect rootsx rootsy
            double xinty[2];
            int ixinty = 0;

            if( xall ) { // x components are everywhere zero
                xinty[0] = rootsy[0];
                xinty[1] = rootsy[1];
                ixinty = irootsy;
            } else if( yall ) { // y components are everywhere zero
                xinty[0] = rootsx[0];
                xinty[1] = rootsx[1];
                ixinty = irootsx;
            } else { // do the intersection
                for( int i = 0; i < irootsx; ++i ) {
                    for( int j = 0; j < irootsy; ++j ) {
                        if( spline::Interpolator<float>::AlmostEqual2sComplement( rootsx[i], rootsy[j], 100000 ) ) {
                            xinty[ixinty] = rootsx[i];
                            ++ixinty;
                        }
                    }
                }
            }

            if( ixinty == 0 ) {
                return 0;
            }

            // intersect xinty and z
            double xintyintz[2];
            int ixintyintz = 0;

            if( xall && yall ) { // xinty components are everywhere zero
                xintyintz[0] = rootsz[0];
                xintyintz[1] = rootsz[1];
                ixintyintz = irootsz;
            } else if( zall ) { // y components are everywhere zero
                xintyintz[0] = xinty[0];
                xintyintz[1] = xinty[1];
                ixintyintz = ixinty;
            } else { // do the intersection
                for( int i = 0; i < ixinty; ++i ) {
                    for( int j = 0; j < irootsz; ++j ) {
                        if( spline::Interpolator<float>::AlmostEqual2sComplement( xinty[i], rootsz[j], 100000 ) ) {
                            xintyintz[ixintyintz] = xinty[i];
                            ++ixintyintz;
                        }
                    }
                }
            }

            *r1 = xintyintz[0];
            *r2 = xintyintz[1];
            return ixintyintz;
        }


        double PBezCarrier::determinant( double a, double b, double c )
        {
            return ( b * b ) - ( 4 * a * c );
        }

        /**
         * Assumes a != 0.
         */
        int PBezCarrier::rootsQuad( double a, double b, double det, double *r1, double *r2 )
        {
            if( spline::Interpolator<double>::AlmostEqual2sComplement( det, 0.f, 10000000 ) ) {
                *r1 = ( -b / ( 2.f * a ) );
                *r2 = *r1;
                return 1;
            }

            if( det < 0.f ) {
                return 0;
            }

            if( a >= 0.f ) {
                *r1 = ( -b - sqrt( det ) ) / ( 2.f * a );
                *r2 = ( -b + sqrt( det ) ) / ( 2.f * a );
            } else {
                *r1 = ( -b + sqrt( det ) ) / ( 2.f * a );
                *r2 = ( -b - sqrt( det ) ) / ( 2.f * a );
            }

            return 2;
        }

        /**
         * Assumes m != 0.
         */
        double PBezCarrier::rootLinear( double m, double c )
        {
            return -c / m;
        }

        int PBezCarrier::roots( double a, double b, double c, double *r1, double *r2 )
        {
            // Not a quadratic
            if( spline::Interpolator<double>::AlmostEqual2sComplement( a, 0.f, 10000000 ) ) {
                // if it not linear
                if( spline::Interpolator<double>::AlmostEqual2sComplement( b, 0.f, 10000000 ) ) {
                    // zero every where
                    if( spline::Interpolator<double>::AlmostEqual2sComplement( c, 0.f, 10000000 ) ) {
                        return -1;
                    } else {        // Zero nowhere
                        return 0;
                    }
                } else {        // Solve the linear eq
                    *r1 = rootLinear( b, c );
                    *r2 = *r1;
                    return 1;
                }
            } else {            // Solve the quadratic
                double det = determinant( a, b, c );
                return rootsQuad( a, b, det, r1, r2 );
            }
        }

        bool PBezCarrier::collinear( const vector<Vec3> &p )
        {
            Vec3 v1 = p[3] - p[1];
            Vec3 v2 = p[2] - p[1];
            Vec3 v3 = p[3] - p[1];
            return parallel( v1, v2 ) && parallel( v1, v3 ) && parallel( v2, v3 );
        }

        bool PBezCarrier::parallel( const Vec3 &v1, const Vec3 &v2 )
        {
            float l12 = v1.SquaredNorm( );
            float l22 = v2.SquaredNorm( );

            if( spline::Interpolator<double>::AlmostEqual2sComplement( l12, 0.f, 1000 ) || spline::Interpolator<double>::AlmostEqual2sComplement( l22, 0.f, 1000 ) ) {
                return true;
            }

            float ctheta = v1.dot( v2 ) / ( sqrt( l12 ) * sqrt( l22 ) );
            return spline::Interpolator<double>::AlmostEqual2sComplement( ctheta, 1.f, 1000 );
        }

    }
}
