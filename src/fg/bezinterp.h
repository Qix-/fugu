/**
 * \file
 * \brief Declares fg::spline::BezInterp
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

#ifndef FG_SPLINE_BEZIER_INTERPOLATOR_H
#define FG_SPLINE_BEZIER_INTERPOLATOR_H

#include <vector>

#include "fg/interpolator.h"

namespace fg {
    namespace spline {

        /**
         * \brief Performs Bezier interpolation.
         */
        template<class T>
        class BezInterp : public Interpolator<T> {
            public:
                BezInterp( const std::vector<T> &controlPoints );
                BezInterp();
                BezInterp( const BezInterp &other );

                BezInterp<T>& operator=( const BezInterp &other );
                virtual ~BezInterp();

                virtual int getDegree() const;
                virtual int getNumSegments() const;

                virtual void setControlPoints( const std::vector<T> &controlPoints );

                virtual void appendControlPoint( const T &cp );

                virtual T getPosition( double t ) const;
                virtual T getDerivative( double t ) const;
                virtual T getSecondDerivative( double t ) const;

                virtual void getDomain( double &min, double &max ) const
                {
                    min = 0.f;
                    max = 1.f;
                }

            protected:
                std::vector<T> mDer1ControlPoint;
                std::vector<T> mDer2ControlPoint;
                double **mChoose;

                virtual void deleteData();
                virtual void allocateChoose();
                virtual void deleteChoose();
        };

    }
}

#include "fg/bezinterp.inl"

#endif
