#ifndef FG_GC_CARRIER_CURVE_H
#define FG_GC_CARRIER_CURVE_H

#include <vector>
#include <iostream>

#include "fg/fg.h"
#include "fg/interpolator.h"

namespace fg{
	namespace gc {
/**
 * \brief Extends the Interpolator class with an orient function and it's derivatives.
 *        To be used as carrier curves of the generalised cylinders.
 */
class CarrierCurve {
public:
    CarrierCurve() {};
    CarrierCurve(const CarrierCurve &other) {  };
    /**
	 * \brief Orients the point (x,y) in R^3 according to
	 *        the position along the carrier curve at value v.
	 */
    virtual Vec3 orient(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDv(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDx(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDy(double v, double x, double y) const = 0;

	virtual const spline::Interpolator<Vec3> * getInterpolator() const = 0;

    virtual void setReferenceFrames(const std::vector<Mat4> &refFrames) = 0;
	virtual std::vector<Mat4> getReferenceFrames() const = 0;
protected:
};

	}
}
#endif
