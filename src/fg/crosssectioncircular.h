#ifndef FG_SPLINE_CROSS_SECTION_CIRCULAR_H
#define FG_SPLINE_CROSS_SECTION_CIRCULAR_H

#include "fg/fg.h"
#include "fg/interpolator.h"

#include "fg/crosssection.h"

namespace fg{
	namespace spline {
/**
 * \brief Provides a variyng cross section function.
 */
class CrossSectionCircular : public CrossSection {
public:
  CrossSectionCircular(double radius = 1.);
  virtual Vec3 getPosition(double u, double v) const; 
  virtual Vec3 getDerivativeU(double u, double v) const;
  virtual Vec3 getDerivativeV(double u, double v) const;
protected:
  double mRadius;
};

	}
}

#endif
