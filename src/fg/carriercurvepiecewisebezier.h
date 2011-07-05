#ifndef FG_SPLINE_CARRIER_CURVE_PIECEWISE_BEZIER_H
#define FG_SPLINE_CARRIER_CURVE_PIECEWISE_BEZIER_H

#include "fg/fg.h"
#include "fg/carriercurve.h"
#include "fg/piecewisebezierinterpolator.h"

#include <vector>
#include <utility>

namespace fg {
	namespace spline {
class CarrierCurvePiecewiseBezier : public CarrierCurve
{
public:
    CarrierCurvePiecewiseBezier(const PiecewiseBezierInterpolator<Vec3> &it);
    virtual Vec3 orient(double v, double x, double y) const;
    virtual Vec3 dOrientDv(double v, double x, double y) const;
    virtual Vec3 dOrientDx(double v, double x, double y) const;
    virtual Vec3 dOrientDy(double v, double x, double y) const;
	virtual const PiecewiseBezierInterpolator<Vec3> & getInterpolator() const;

private:
    virtual void getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const;
    virtual double getQuasiNormalTheta(double t) const;

	std::vector< std::pair<double, double> > mNormalTheta;
	std::vector< std::pair<int, Vec3> > mLinearSegs;
	const PiecewiseBezierInterpolator<Vec3> &mInterpolator;

// These should go somewhere else!
public:
    static int findInflectionPointsVec( const Vec3 *cp, double *i1, double *i2 );
	static double determinant(double a, double b, double c);
	static int rootsQuad(double a, double b, double det, double *r1, double *r2);
	static double rootLinear(double m, double c);
	static int roots(double a, double b, double c, double *r1, double *r2);
    static int roots( Vec3 a, Vec3 b, Vec3 c, double *r1, double *r2 );
	static bool collinear( const Vec3 *p );
	static bool parallel( const Vec3 &v1, const Vec3 &v2 );
};
	}
}

#endif
