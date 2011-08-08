/**
 * \file
 * \brief Declares fg::gc::ScaleInterpCrossSec
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

#ifndef FG_GC_SCALED_INTERPOLATOR_CROSS_SECTION_H
#define FG_GC_SCALED_INTERPOLATOR_CROSS_SECTION_H

#include "fg/fg.h"

#include "fg/gc/crosssection.h"
#include "fg/gc/interpolator.h"

namespace fg {
    namespace gc {
        /**
         * \brief Provides a cross section defined by an interpolator which is scaled by another interpolator.
         */
        class ScaleInterpCrossSec : public CrossSection {
            public:
                ScaleInterpCrossSec( const Interpolator<Vec3> &interp, const Interpolator<double> &scale );
                virtual Vec3 getPosition( double u, double v ) const;
                virtual Vec3 getDerivativeU( double u, double v ) const;
                virtual Vec3 getDerivativeV( double u, double v ) const;

                virtual std::vector<Vec3> getCrossSection( double v, double scale = 1. ) const;
            protected:
				const Interpolator<Vec3> &mInterpolator;
				const Interpolator<double> &mScale;
        };

    }
}

#endif
