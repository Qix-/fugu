/**
 * \file
 * \brief Declares fg::gc::MorphCrossSec
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

#ifndef FG_GC_MORPHING_CROSS_SECTION_H
#define FG_GC_MORPHING_CROSS_SECTION_H

#include "fg/fg.h"

#include "fg/gc/crosssection.h"
#include "fg/gc/pbezinterpdiv.h"
#include "fg/gc/bezinterp.h"

namespace fg {
    namespace gc {
        /**
         * \brief Provides a constant cross section defined by an interpolator.
         */
        class MorphCrossSec : public CrossSection {
            public:
                MorphCrossSec( const std::vector< PBezInterpDiv > &interps );
                virtual Vec3 getPosition( double u, double v ) const;
                virtual Vec3 getDerivativeU( double u, double v ) const;
                virtual Vec3 getDerivativeV( double u, double v ) const;

                virtual std::vector<Vec3> getCrossSection( double v, double scale = 1. ) const;
                virtual PBezInterpDiv getCrossSectionInterp( double v ) const;
            protected:
				virtual void addSegment( const PBezInterpDiv &first, const PBezInterpDiv &second );

				std::vector< std::vector<Vec3> > mSourceCP;
				std::vector< std::vector< std::pair<Vec3,Vec3> > > mSourceGrad1;

				std::vector< std::vector<Vec3> > mAttractorCP;
				std::vector< std::vector< std::pair<Vec3,Vec3> > > mAttractorGrad1;

				static const double mThreashold = 5E-2;

			public:
				static double getSpeed( double t, const BezInterp<Vec3> &spline );
			    static double getLength( const BezInterp<Vec3> &spline, double t0, double t1 );
				static double getTotalLength( const PBezInterpDiv &spline );
				static double getTime( const BezInterp<Vec3> &spline, double length, int iterations = 32, double tolerance = 1e-06 );
				static double RombergIntegral( int order, double a, double b,
                                               const BezInterp<Vec3> &data );
				static PBezInterpDiv addPoint( const PBezInterpDiv &spline, int seg, float localT );
        };

    }
}

#endif
