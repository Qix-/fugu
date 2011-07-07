#include "fg/fg.h"
#include "fg/mat4.h"
#include "fg/carriercurvelinear.h"

namespace fg {
namespace gc {

CarrierCurveLinear::CarrierCurveLinear(const std::vector<Mat4> &refFrames)
:mInterpolator(NULL)
,mOrients(NULL)
{
	setControlPoints(refFrames);
}

void CarrierCurveLinear::setControlPoints(const std::vector<Mat4> &refFrames)
{
	deleteData();

	std::vector<Vec3> tmpCp;

    for (int i = 0; i < refFrames.size(); ++i)
	{
	    // Get the control points and headings PUT THIS INTO MAT4!!
	    Vec3 p1 = refFrames[i]   * Vec3(0.,0.,0.); // The pos
        tmpCp.push_back(p1);
	}

	mInterpolator = new spline::LinearInterpolator<Vec3>(tmpCp);
	mOrients = new spline::LinearInterpolator<Mat4>(refFrames);
}

CarrierCurveLinear::~CarrierCurveLinear()
{
    deleteData();
}

void CarrierCurveLinear::deleteData()
{
    if(mInterpolator)
        delete mInterpolator;

	if(mOrients)
		delete mOrients;

    mInterpolator = NULL;
	mOrients = NULL;
}

const spline::LinearInterpolator<Vec3> * CarrierCurveLinear::getInterpolator() const
{
    return mInterpolator;
}

void CarrierCurveLinear::getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const
{
	if (H)
		*H = mOrients->getPosition(t) * Vec3(0.,0.,1.) - mInterpolator->getPosition(t);
	if (U)
		*U = mOrients->getPosition(t) * Vec3(1.,0.,0.) - mInterpolator->getPosition(t);
	if (L)
		*L = mOrients->getPosition(t) * Vec3(0.,1.,0.) - mInterpolator->getPosition(t);
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
