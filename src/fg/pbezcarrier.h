/**
 * \file
 * \brief Declares fg::gc::PBezCarrier
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


#ifndef FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H
#define FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H

#include <vector>
#include <utility>

#include "fg/fg.h"
#include "fg/carriercurve.h"
#include "fg/pbezinterp.h"

namespace fg {
    namespace gc {
        class PBezCarrier : public CarrierCurve
        {
            public:
                PBezCarrier( const std::vector<Mat4> &refFrames, const std::vector< std::pair<double, double> > &stiffness );
                PBezCarrier( const std::vector<Mat4> &refFrames );
                PBezCarrier( const PBezCarrier &other );
                PBezCarrier &operator=( const PBezCarrier &other );
                PBezCarrier();
                ~PBezCarrier();

                virtual Quat orient( double v ) const;
                virtual Quat orientDer( double v ) const;

                virtual std::pair<Quat, Quat> getSegOrients( int seg ) const;

                virtual const spline::PBezInterp<Vec3> * getInterpolator() const;

                virtual void setReferenceFrames( const std::vector<Mat4> &refFrames, const std::vector< std::pair<double, double> > &stiffness );
                virtual void setReferenceFrames( const std::vector<Mat4> &refFrames );
                virtual std::vector<Mat4> getReferenceFrames() const;
            private:
                virtual Quat getFrenetFrame( double v ) const;
                void updateInflectionPoints( int seg );
                void deleteData();

                std::vector<int>  mSegType;
                std::vector< std::pair <double, double> > mInflectionPoints;
                std::vector<Mat4> mRFrames;

                spline::PBezInterp<Vec3> * mInterpolator;

// These should go somewhere else!
            public:
                static int findInflectionPointsVec( const std::vector<Vec3> &cp, double *i1, double *i2 );
                static double determinant( double a, double b, double c );
                static int rootsQuad( double a, double b, double det, double *r1, double *r2 );
                static double rootLinear( double m, double c );
                static int roots( double a, double b, double c, double *r1, double *r2 );
                static int roots( Vec3 a, Vec3 b, Vec3 c, double *r1, double *r2 );
                static bool collinear( const std::vector< Vec3 > &p );
                static bool parallel( const Vec3 &v1, const Vec3 &v2 );
        };
    }
}

#endif
