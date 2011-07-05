#include "crosssectioncircular.h"

namespace fg{
	namespace spline {
		
  CrossSectionCircular::CrossSectionCircular(double radius)
  {
	  mRadius = radius;
  }
  Vec3 CrossSectionCircular::getPosition(double u, double v) const
  {
	  return Vec3(mRadius * cos(u), mRadius * sin(u), 0.);
  }
  Vec3 CrossSectionCircular::getDerivativeU(double u,double v) const
  {
	  return Vec3(-mRadius * sin(u), mRadius * cos(u), 0.);
  }
  Vec3 CrossSectionCircular::getDerivativeV(double u,double v) const
  {
	  return Vec3(0., 0., 0.);
  }
	}
}
