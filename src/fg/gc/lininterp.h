/**
 * \file
 * \brief Declares fg::gc::LinInterp
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

#ifndef FG_SPLINE_LINEAR_INTERPOLATOR_H
#define FG_SPLINE_LINEAR_INTERPOLATOR_H

#include <vector>
#include <utility>

#include "fg/fg.h"

#include "fg/gc/interpolator.h"

namespace fg {
    namespace gc {
        /**
         * \brief A class for perfoming linear interpolation.
         */
        template<class T> class LinInterp : public Interpolator< T > {
            public:
                /**
                 * \brief Constructs a linear interpolator for the given points.
                 */
                LinInterp( const std::vector<T> &mControlPoints );
                /**
                 * \brief Constructs an invalid interpolator.
                 */
                LinInterp();

                /**
                 * \brief Performs a deep copy.
                 */
                LinInterp( const LinInterp<T> &other );

                /**
                 * \brief Performs a deep copy.
                 */
                LinInterp<T>& operator=( const LinInterp<T> &other );

                virtual T getPosition( double t ) const;

                virtual T getDerivative( double t ) const;

                virtual T getSecondDerivative( double t ) const;

                virtual T *getApprox( int &n ) const;
                virtual std::vector<T> getApproxVector( int &n ) const;

                virtual void getDomain( double &min, double &max ) const;

                virtual int getNumSegments() const;

				virtual Interpolator<T> * scale( double s ) const;

            private:
                int getControlPointIndex( double t ) const;
                double getInternalT( double externalT ) const;
        };

    }
}

#include "fg/gc/lininterp.inl"
#endif
