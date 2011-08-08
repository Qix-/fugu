/**
 * \file
 * \brief Defines fg::gc::InterpCrossSec
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

#include "fg/gc/interpcrosssec.h"

using namespace std;

namespace fg {
    namespace gc {

        InterpCrossSec::InterpCrossSec( const Interpolator<Vec3> &interp )
		:mInterpolator(interp)
        {
        }
        Vec3 InterpCrossSec::getPosition( double u, double v ) const
        {
			mInterpolator.getPosition(u);
        }
        Vec3 InterpCrossSec::getDerivativeU( double u, double v ) const
        {
			mInterpolator.getDerivative(u);
        }
        Vec3 InterpCrossSec::getDerivativeV( double u, double v ) const
        {
            return Vec3( 0., 0., 0. );
        }

        vector<Vec3> InterpCrossSec::getCrossSection( double v, double scale ) const
        {
			int n = -1;
			Interpolator<Vec3> * it = mInterpolator.scale( scale );
			vector<Vec3> data = it->getApproxVector( n );
			delete it;
			return data;
        }

    }
}
