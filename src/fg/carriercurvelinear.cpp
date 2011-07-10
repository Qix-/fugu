#include "fg/fg.h"
#include "fg/mat4.h"
#include "fg/carriercurvelinear.h"

//#include <iostream>

namespace fg {
namespace gc {

CarrierCurveLinear::CarrierCurveLinear(const std::vector<Mat4> &refFrames)
:mInterpolator(NULL)
{
	setReferenceFrames(refFrames);
}

CarrierCurveLinear::CarrierCurveLinear(const CarrierCurveLinear &other)
:mInterpolator(NULL)
{
	*this = other;
}

CarrierCurveLinear& CarrierCurveLinear::operator=(const CarrierCurveLinear &other)
{
	setReferenceFrames( other.getReferenceFrames() );
	return *this;
}

std::vector<Mat4> CarrierCurveLinear::getReferenceFrames() const
{
	return mRFrames;
}

void CarrierCurveLinear::setReferenceFrames(const std::vector<Mat4> &refFrames)
{
	deleteData();

	std::vector<Vec3> tmpCp;

    for (int i = 0; i < refFrames.size(); ++i)
	{
	    // Get the control points location PUT THIS INTO MAT4 OR RFRAME OR SOMETHING!!
	    Vec3 p1 = refFrames[i]   * Vec3(0.,0.,0.); // The pos

		//std::cout << refFrames[i] << std::endl;

        tmpCp.push_back(p1);
		// Get the rotation
		mOrients.push_back( Quat( refFrames[i] ) );
		mRFrames.push_back( refFrames[i] );

		//std::cout << mOrients[i] << std::endl;
	}

	mInterpolator = new spline::LinearInterpolator<Vec3>(tmpCp);
}

CarrierCurveLinear::~CarrierCurveLinear()
{
    deleteData();
}

void CarrierCurveLinear::deleteData()
{
	mOrients.clear();
	mRFrames.clear();
    if(mInterpolator)
        delete mInterpolator;

    mInterpolator = NULL;
}

const spline::LinearInterpolator<Vec3> * CarrierCurveLinear::getInterpolator() const
{
    return mInterpolator;
}

void CarrierCurveLinear::getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const
{
	int tint = (int) t;
	tint = clamp<int>( tint, 0, mInterpolator->getNumSegments() - 1 );
	double tfrac = t - tint;
	Quat rot;

	//std::cout << "t = " << t << ", tint = " << tint << ", tfrac = " << tfrac <<std::endl;

    //std::cout << mOrients[2] << std::endl;
	rot = mOrients[tint].slerp(tfrac, mOrients[tint+1]);
	if (H)
		*H = rot * Vec3(0.,0.,1.);
	if (U)
		*U = rot * Vec3(1.,0.,0.);
	if (L)
		*L = rot * Vec3(0.,1.,0.);
	/*
	Vec3 tangent = mInterpolator->getDerivative(t);
	tangent.normalise();

	Vec3 normal;

	if ( tangent.dot( Vec3(0.,1.,0.) ) > 1.f ) {
		 normal = tangent^Vec3(1., 0., 0.);
	} else {
		 normal = tangent^Vec3(0., 1., 0.);
	}

	Vec3 biNorm = normal^tangent;

	if (H)
		*H = tangent;
	if (U)
		*U = normal;
	if (L)
		*L = biNorm;
		*/
}

Vec3 CarrierCurveLinear::orient(double v, double x, double y) const
{
    Vec3 U, B;
    getOrientation(v, 0, &U, &B);
    return U*x + B*y;
}

Vec3 CarrierCurveLinear::dOrientDv(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

Vec3 CarrierCurveLinear::dOrientDx(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

Vec3 CarrierCurveLinear::dOrientDy(double v, double x, double y) const
{
    return Vec3(0.,0.,0.);
}

}
}
