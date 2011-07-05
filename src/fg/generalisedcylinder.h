#ifndef FG_SPLINE_GENERALISED_CYLINDER_H
#define FG_SPLINE_GENERALISED_CYLINDER_H

#include <boost/shared_ptr.hpp>

#include "fg/fg.h"
#include "fg/interpolator.h"

#include "fg/carriercurve.h"
#include "fg/crosssection.h"

namespace fg{
	namespace spline {
/**
 * \brief Provides a variyng cross section function.
 */
class GeneralisedCylinder {
public:
  GeneralisedCylinder(const CarrierCurve &carrier, const CrossSection &crossSection);
  boost::shared_ptr<Mesh> createMesh(int n = 10, int m = 10) const;

protected:
  const CarrierCurve &mCarrier;
  const CrossSection &mCrossSection;
};

	}
}
#endif
