/**
 * \file
 * \brief Declares fg::gc::LinCarrier
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


#ifndef FG_GC_CARRIER_CURVE_LINEAR_H
#define FG_GC_CARRIER_CURVE_LINEAR_H

#include <vector>
#include <utility>

#include "fg/fg.h"
#include "fg/quat.h"

#include "fg/gc/carriercurve.h"
#include "fg/gc/lininterp.h"

namespace fg {
    namespace gc {
        class LinCarrier : public CarrierCurve
        {
            public:
                LinCarrier( const std::vector<Mat4> &refFrames );
                LinCarrier( const LinCarrier &other );
                LinCarrier &operator=( const LinCarrier &other );
                ~LinCarrier();

                virtual Quat orient( double v ) const;
                virtual Quat orientDer( double v ) const;

                virtual std::pair<Quat, Quat> getSegOrients( int seg ) const;

                virtual const LinInterp<Vec3> * getInterpolator() const;

                virtual void setReferenceFrames( const std::vector<Mat4> &refFrames );
                virtual std::vector<Mat4> getReferenceFrames() const;
            private:
//    virtual void getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const;
                void deleteData();

                LinInterp<Vec3> * mInterpolator;
                std::vector<Mat4> mRFrames;
        };
    }
}

#endif
