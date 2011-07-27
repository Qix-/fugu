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

        vector<Vec3> ScaleInterpCrossSec::getCrossSection( double v, double scale ) const
        {
			int n = -1;
			spline::Interpolator<Vec3> * it = mInterpolator.scale( mScale.getPosition( v ) );
			spline::Interpolator<Vec3> * it2 = it->scale( scale );
			vector<Vec3> data = it2->getApproxVector( n );
			delete it;
			delete it2;
			return data;
        }

    }
}
