/**
 * \file
 * \brief Declares fg::spline::CRInterp
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

#ifndef FG_SPLINE_CATMULL_ROM_INTERPOLATOR_H
#define FG_SPLINE_CATMULL_ROM_INTERPOLATOR_H

#include <vector>

#include "fg/interpolator.h"

namespace fg {
    namespace spline {

        /**
         * \brief Performs Catmull Rom interpolation.
         */
        template<class T>
        class CRInterp : public Interpolator<T> {
            public:
                CRInterp( const std::vector<T> &controlPoints );
                CRInterp();
                CRInterp( const CRInterp &other );

                CRInterp<T>& operator=( const CRInterp &other );
                virtual ~CRInterp();

                virtual int getNumSegments() const;

                virtual void setControlPoints( const std::vector<T> &controlPoints );

                virtual void appendControlPoint( const T &cp );

                virtual T getPosition( double t ) const;
                virtual T getDerivative( double t ) const;
                virtual T getSecondDerivative( double t ) const;

                virtual void getDomain( double &min, double &max ) const;

				virtual Interpolator<T> * scale( double s ) const;

            protected:
				void getLocalControlPoints( double t, double &tL, T &P0, T &P1, T &P2, T &P3 ) const;
				void getCoeff( T &P0, T &P1, T &P2, T &P3, T *A, T *B, T *C, T *D ) const;
        };

    }
}

#include "fg/crinterp.inl"

#endif
