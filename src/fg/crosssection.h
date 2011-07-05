#ifndef FG_SPLINE_CROSS_SECTION_H
#define FG_SPLINE_CROSS_SECTION_H

#include "fg/fg.h"
#include "fg/interpolator.h"

namespace fg{
	namespace spline {
/**
 * \brief Provides a variyng cross section function.
 */
class CrossSection {
public:
  virtual Vec3 getPosition(double u, double v) const = 0;
  virtual Vec3 getDerivativeU(double u, double v) const = 0;
  virtual Vec3 getDerivativeV(double u, double v) const = 0;
protected:
};

	}
}
#endif
