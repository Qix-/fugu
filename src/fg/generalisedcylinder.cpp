#include "generalisedcylinder.h"

#include <iostream>

using namespace std;

namespace fg {
namespace spline {
GeneralisedCylinder::GeneralisedCylinder(const CarrierCurve &carrier, const CrossSection &crossSection)
:mCarrier(carrier)
,mCrossSection(crossSection)
{
}

boost::shared_ptr<Mesh> GeneralisedCylinder::createMesh(int n, int m) const
{
	fg::Mesh::MeshBuilder mb;

    double vmin, vmax;
    mCarrier.getInterpolator().getDomain(vmin, vmax);
	double vinc = (vmax-vmin) / ((double) m);
	double uinc = (2.*M_PI) / ((double) n);
	double v = 1.;
	double u = 0.;
    for (unsigned int j = 0; j <= m; ++j) {
        for (unsigned int i = 0; i < n; ++i) {
			Vec3 cs = mCrossSection.getPosition(u,v);
			Vec3 pos = mCarrier.getInterpolator().getPosition(v) + mCarrier.orient(v, cs.getX(), cs.getY());
		    mb.addVertex(pos.getX(),pos.getY(),pos.getZ());
			u += uinc;
        }
	    v += vinc;
		u = 0.f;
    }


    for (unsigned int j = 0; j < m; ++j) {
        for (unsigned int i = 0; i < n; ++i) {
            mb.addFace(i+j*n, i+(j+1)*n, (i+1)%(n)+j*n);
            mb.addFace(i+(j+1)*n, (i+1)%(n)+(j+1)*n, (i+1)%(n)+j*n);
        }
    }

	return mb.createMesh();
}
}
}
