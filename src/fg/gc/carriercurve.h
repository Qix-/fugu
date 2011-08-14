/**
 * \file
 * \brief Declares fg::gc::CarrierCurve
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


#ifndef FG_GC_CARRIER_CURVE_H
#define FG_GC_CARRIER_CURVE_H

#include <vector>
#include <utility>
#include <iostream>

#include "fg/fg.h"

#include "fg/gc/interpolator.h"
#include "fg/quat.h"

namespace fg {
    namespace gc {
        /**
         * \brief Extends the Interpolator class with an orient function and it's derivatives.
         *        To be used as carrier curves of the generalised cylinders.
         */
        class CarrierCurve {
            public:
                CarrierCurve() {};
                CarrierCurve( const CarrierCurve &other ) {  };
                /**
                 * \brief Returns a quaternion representing the frame of the
                 *        curve at parameter value v.
                 */
                virtual Quat orient( double v ) const = 0;
                /**
                 * \brief Returns a quaternion representing rate of change of the frame of the
                 *        curve at parameter value v.
                 */
                virtual Quat orientDer( double v ) const = 0;

                /**
                 * \brief Returns the orientation at either end of the specified segment.
                 */
                virtual std::pair<Quat, Quat> getSegOrients( int seg ) const = 0;

                virtual const Interpolator<Vec3> * getInterpolator() const = 0;

                virtual void setReferenceFrames( const std::vector<Mat4> &refFrames ) = 0;
                virtual std::vector<Mat4> getReferenceFrames() const = 0;
            protected:
        };

    }
}
#endif
