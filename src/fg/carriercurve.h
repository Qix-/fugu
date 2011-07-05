#ifndef FG_SPLINE_CARRIER_CURVE_H
#define FG_SPLINE_CARRIER_CURVE_H

#include "fg/fg.h"
#include "fg/interpolator.h"

namespace fg{
	namespace spline {
/**
 * \brief Extends the Interpolator class with an orient function and it's derivatives.
 *        To be used as carrier curves of the generalised cylinders.
 */
class CarrierCurve {
public:
    /**
	 * \brief Orients the point (x,y) in R^3 according to
	 *        the position along the carrier curve at value v.
	 */
    virtual Vec3 orient(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDv(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDx(double v, double x, double y) const = 0;
    virtual Vec3 dOrientDy(double v, double x, double y) const = 0;

	virtual const Interpolator<Vec3> & getInterpolator() const = 0;
protected:
};

	}
}
#endif
