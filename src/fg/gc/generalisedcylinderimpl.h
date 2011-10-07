#ifndef FG_GC_GENERALISED_CYLINDER_IMPL_H
#define FG_GC_GENERALISED_CYLINDER_IMPL_H

#include <vector>
#include <utility>

#include "fg/vec3.h"
#include "fg/gc/interpolator.h"
#include "fg/gc/carriercurve.h"
#include "fg/gc/crosssection.h"

namespace fg {
	namespace gc {

/**
 * \brief Uses a CarrierCurve and a CrossSection to define a surface.
 */
class GeneralisedCylinderImpl {
    public:
		GeneralisedCylinderImpl( double r = 1. );
		//GeneralisedCylinderImpl( const CarrierCurve &carrier, const CrossSection &crossSection );
        Vec3 getPosition( double u, double v ) const;
		Vec3 getDerU( double u, double v ) const;
		Vec3 getDerV( double u, double v ) const;
		Vec3 getNorm( double u, double v ) const;
		Vec3 getNormU( double u, double v ) const;
		Vec3 getNormV( double u, double v ) const;

		double impl( Vec3 x, double &u, double &v ) const;
		void impl( Vec3 x, double &u_i, double &v_i, double &d, Vec3 &grad ) const;

		void sim(double &x, double &y, double &z,
                 double a, double b, double c, double d,
              	 double l, double m, double n, double k,
                 double p, double q, double r, double s) const;

    protected:
        //const CarrierCurve &mCarrier;
        //const CrossSection &mCrossSection;
		double mRadius;
		const static int mMaxIts = 100;
		const static double mThreshHold = 1E-30;
};

	}
}
#endif
