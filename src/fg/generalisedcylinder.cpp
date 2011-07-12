#include "generalisedcylinder.h"

#include <iostream>

using namespace std;

namespace fg {
namespace gc {
GeneralisedCylinder::GeneralisedCylinder(const CarrierCurve &carrier, const CrossSection &crossSection, const vector<Mat4> &orients)
:mCarrier(carrier)
,mCrossSection(crossSection)
{
	std::vector<Quat> newOrients;

	for (int i = 0; i < orients.size(); ++i)
	{
		newOrients.push_back( Quat( orients[i] ) );
	}
	updateOrients( newOrients );
}

void GeneralisedCylinder::updateOrients( const vector<Quat> &orients )
{
	mOrients.clear();
	// Store the difference between the specified orientations and the one supplied by the carrier curve
	for(int i = 0; i < mCarrier.getInterpolator()->getNumSegments(); ++i)
	{
		pair<Quat,Quat> co = mCarrier.getSegOrients(i);
		pair<Quat,Quat> no;
		no.first = orients[i]*co.first.inverse();
		no.second = orients[i+1]*co.second.inverse();
		mOrients.push_back(no);
	}
}

Vec3 GeneralisedCylinder::getPosition( double u, double v ) const
{
	// Position on cross section
	Vec3 cs = mCrossSection.getPosition(u,v);

	// Carriers frame rotation
	Quat cr = mCarrier.orient(v);

	// Additional rotation
	int vint = (int) v;
	vint = clamp<int>(vint, 0, mCarrier.getInterpolator()->getNumSegments() - 1 );
	double vfrac = v - vint;
	Quat ar = mOrients[vint].first.slerp( vfrac, mOrients[vint].second );

	// Put it all together
	return mCarrier.getInterpolator()->getPosition(v) + ar * cr * cs;
}

boost::shared_ptr<Mesh> GeneralisedCylinder::createMesh(int n, int m) const
{
	fg::Mesh::MeshBuilder mb;

    double vmin, vmax;
    mCarrier.getInterpolator()->getDomain(vmin, vmax);
	double vinc = (vmax-vmin) / ((double) m);
	double uinc = (2.*M_PI) / ((double) n);
	double v = 0.;
	double u = 0.;
    for (unsigned int j = 0; j <= m; ++j) {
        for (unsigned int i = 0; i < n; ++i) {
			Vec3 pos = getPosition(u,v);
		    mb.addVertex(pos.getX(),pos.getY(),pos.getZ());
			u += uinc;
        }
	    v += vinc;
		u = 0.f;
    }


    for (unsigned int j = 0; j < m; ++j) {
        for (unsigned int i = 0; i < n; ++i) {
            mb.addFace((i+1)%(n)+j*n, i+(j+1)*n, i+j*n);
            mb.addFace((i+1)%(n)+j*n, (i+1)%(n)+(j+1)*n, i+(j+1)*n);

            //mb.addFace(i+j*n, i+(j+1)*n, (i+1)%(n)+j*n);
            //mb.addFace(i+(j+1)*n, (i+1)%(n)+(j+1)*n, (i+1)%(n)+j*n);
        }
    }

	return mb.createMesh();
}
}
}
