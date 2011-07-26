/**
 * \file
 * \brief Defines fg::gc::ScaleInterpCrossSec
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

#include "fg/scaleinterpcrosssec.h"

using namespace std;

namespace fg {
    namespace gc {

        ScaleInterpCrossSec::ScaleInterpCrossSec( const spline::Interpolator<Vec3> &interp, const spline::Interpolator<double> &scale )
		:mInterpolator(interp)
		,mScale(scale)
        {
        }
        Vec3 ScaleInterpCrossSec::getPosition( double u, double v ) const
        {
			return mInterpolator.getPosition(u) * mScale.getPosition( v );
        }
        Vec3 ScaleInterpCrossSec::getDerivativeU( double u, double v ) const
        {
			return mInterpolator.getDerivative(u) * mScale.getPosition( v );
        }
        Vec3 ScaleInterpCrossSec::getDerivativeV( double u, double v ) const
        {
			return mInterpolator.getPosition(u) * mScale.getDerivative( v );
        }

        vector<Vec3> ScaleInterpCrossSec::getCrossSection( double v ) const
        {
			int n = -1;
			spline::Interpolator<Vec3> * it = mInterpolator.scale( mScale.getPosition( v ) );
			vector<Vec3> data = it->getApproxVector( n );
			delete it;
			return data;
        }

    }
}
