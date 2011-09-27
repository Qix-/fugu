#ifndef FG_GC_FRONT_POLY_H
#define FG_GC_FRONT_POLY_H

#include <list>
#include <utility>

#include "fg/vec3.h"

namespace fg {
	namespace gc {
/**
 * A single point on the front polygon
 */
class P {
	public:
		P( Vec3 pos, Vec3 norm, Vec3 t1, Vec3 t2, double u, double v, double f );
		P();
		Vec3 mPos;
		Vec3 mNorm;
		Vec3 mT1, mT2;
		double mFrontAngle;
		double mU, mV, mF;
		bool mChanged;
		bool mDistCheck;
};

/**
 * \brief 
 */
class FrontPoly : std::list<P> {
};

}
}
#endif
