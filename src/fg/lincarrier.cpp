/**
 * \file
 * \brief Defines fg::gc::LinCarrier
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
#include "fg/lincarrier.h"

//#include <iostream>

namespace fg {
    namespace gc {

        LinCarrier::LinCarrier( const std::vector<Mat4> &refFrames )
            : mInterpolator( NULL )
        {
            setReferenceFrames( refFrames );
        }

        LinCarrier::LinCarrier( const LinCarrier &other )
            : mInterpolator( NULL )
        {
            *this = other;
        }

        LinCarrier &LinCarrier::operator=( const LinCarrier &other )
        {
            setReferenceFrames( other.getReferenceFrames() );
            return *this;
        }

        std::vector<Mat4> LinCarrier::getReferenceFrames() const
        {
            return mRFrames;
        }

        void LinCarrier::setReferenceFrames( const std::vector<Mat4> &refFrames )
        {
            deleteData();
            std::vector<Vec3> tmpCp;

            for( int i = 0; i < refFrames.size(); ++i )
            {
                // Get the control points location PUT THIS INTO MAT4 OR RFRAME OR SOMETHING!!
                Vec3 p1 = refFrames[i]   * Vec3( 0., 0., 0. ); // The pos
                //std::cout << refFrames[i] << std::endl;
                tmpCp.push_back( p1 );
                // Get the rotation
                mRFrames.push_back( refFrames[i] );
                //mOrients.push_back( Quat( refFrames[i] ) );
                //std::cout << mOrients[i] << std::endl;
            }

            mInterpolator = new spline::LinInterp<Vec3>( tmpCp );
        }

        LinCarrier::~LinCarrier()
        {
            deleteData();
        }

        void LinCarrier::deleteData()
        {
            mRFrames.clear();

            if( mInterpolator )
                delete mInterpolator;

            mInterpolator = NULL;
        }

        const spline::LinInterp<Vec3> * LinCarrier::getInterpolator() const
        {
            return mInterpolator;
        }

        std::pair<Quat, Quat> LinCarrier::getSegOrients( int seg ) const
        {
            return std::pair<Quat, Quat>( Quat( ), Quat( ) );
        }

        Quat LinCarrier::orient( double v ) const
        {
            return Quat( );
            /*
            Vec3 tan = mInterpolator->getDerivative(v);
            tan.normalise();

            Vec3 norm;

            if ( tan.dot( Vec3(0.,1.,0.) ) > 1.f ) {
                 norm = tan.cross(Vec3(1., 0., 0.));
            } else {
                 norm = tan.cross(Vec3(0., 1., 0.));
            }

            Vec3 biNorm = norm.cross(tan);

            return Quat( norm, biNorm, tan );
            */
        }

        Quat LinCarrier::orientDer( double v ) const
        {
            return Quat();
        }

//void LinCarrier::getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const
//{
//  int tint = (int) t;
//  tint = clamp<int>( tint, 0, mInterpolator->getNumSegments() - 1 );
//  double tfrac = t - tint;
//  Quat rot;
//
//  //std::cout << "t = " << t << ", tint = " << tint << ", tfrac = " << tfrac <<std::endl;
//
//    //std::cout << mOrients[2] << std::endl;
//  rot = mOrients[tint].slerp(tfrac, mOrients[tint+1]);
//  if (H)
//      *H = rot * Vec3(0.,0.,1.);
//  if (U)
//      *U = rot * Vec3(1.,0.,0.);
//  if (L)
//      *L = rot * Vec3(0.,1.,0.);
//  /*
//  Vec3 tangent = mInterpolator->getDerivative(t);
//  tangent.normalise();
//
//  Vec3 normal;
//
//  if ( tangent.dot( Vec3(0.,1.,0.) ) > 1.f ) {
//       normal = tangent^Vec3(1., 0., 0.);
//  } else {
//       normal = tangent^Vec3(0., 1., 0.);
//  }
//
//  Vec3 biNorm = normal^tangent;
//
//  if (H)
//      *H = tangent;
//  if (U)
//      *U = normal;
//  if (L)
//      *L = biNorm;
//      */
//}
//
//Vec3 LinCarrier::orient(double v, double x, double y) const
//{
//    Vec3 U, B;
//    getOrientation(v, 0, &U, &B);
//    return U*x + B*y;
//}
//
//Vec3 LinCarrier::dOrientDv(double v, double x, double y) const
//{
//    return Vec3(0.,0.,0.);
//}
//
//Vec3 LinCarrier::dOrientDx(double v, double x, double y) const
//{
//    return Vec3(0.,0.,0.);
//}
//
//Vec3 LinCarrier::dOrientDy(double v, double x, double y) const
//{
//    return Vec3(0.,0.,0.);
//}
//
    }
}
