/**
 * \file
 * \brief Declares fg::gc::PBezInerpDiv
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

#ifndef FG_SPLINE_PIECEWISE_BEZIER_DIV_H
#define FG_SPLINE_PIECEWISE_BEZIER_DIV_H

#include "fg/gc/pbezinterp.h"

namespace fg {
    namespace gc {
        /**
         * \brief Adds recursive subdivsion to PBezInterp.
         */
        class PBezInterpDiv : public PBezInterp<Vec3> {
			public:
              PBezInterpDiv( const std::vector<Vec3> &controlPoints)
			  :PBezInterp<Vec3>( controlPoints )
			  {
			  }

              PBezInterpDiv( const std::vector<Vec3> &controlPoints, const std::vector< std::pair<Vec3, Vec3> > &gradients )
			  :PBezInterp<Vec3>( controlPoints, gradients )
			  {
			  }

              PBezInterpDiv( const PBezInterpDiv &other )
			  :PBezInterp<Vec3>( other )
			  {
			  }

              PBezInterpDiv( )
			  :PBezInterp<Vec3>( )
			  {}

              PBezInterpDiv& operator=( const PBezInterpDiv &other )
			  {
				  PBezInterp<Vec3>::operator=(other);
				  return *this;
			  }

			  /**
			   * Uses recursive subdivision to approximate the curve.
			   */
			  virtual std::vector<Vec3> getApproxVector( int &n ) const;

			  Interpolator<Vec3> * scale( double s ) const;
		};
	}
}
#endif
