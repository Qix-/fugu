#include "fg/gc/frontpoly.h"

namespace fg {
	namespace gc {

P::P( Vec3 pos, Vec3 norm, Vec3 t1, Vec3 t2, double u, double v, double f ) {
	mPos = pos;
	mNorm = norm;
	mT1 = t1;
	mT2 = t2;
	mDistCheck = true;
	mU = u;
	mV = v;
	mF = f;
}

P::P() {
}

	}
}
