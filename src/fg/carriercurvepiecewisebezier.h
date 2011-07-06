#ifndef FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H
#define FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H

#include "fg/fg.h"
#include "fg/carriercurve.h"
#include "fg/piecewisebezierinterpolator.h"
#include "fg/linearinterpolator.h"

#include <vector>
#include <utility>

namespace fg {
	namespace gc {
class CarrierCurvePiecewiseBezier : public CarrierCurve
{
public:
    CarrierCurvePiecewiseBezier(int numControlPoints, const Mat4 *refFrames, const std::pair<double,double> *stiffness);
    CarrierCurvePiecewiseBezier(int numControlPoints, const Mat4 *refFrames);
	~CarrierCurvePiecewiseBezier();
    virtual Vec3 orient(double v, double x, double y) const;
    virtual Vec3 dOrientDv(double v, double x, double y) const;
    virtual Vec3 dOrientDx(double v, double x, double y) const;
    virtual Vec3 dOrientDy(double v, double x, double y) const;
	virtual const spline::PiecewiseBezierInterpolator<Vec3> * getInterpolator() const;

    virtual void setControlPoints(int numControlPoints, const Mat4 *refFrames, const std::pair<double,double> *stiffness);
private:
    virtual void getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const;
    virtual void getFrenetFrame(double t, Vec3 *H, Vec3 *U, Vec3 *L) const;
    void updateInflectionPoints( int seg );
	void deleteData();

    int * mSegType;
	std::pair <double, double> * mInflectionPoints;

	spline::PiecewiseBezierInterpolator<Vec3> * mInterpolator;
	spline::LinearInterpolator<double> * mAlphaInt;
	spline::LinearInterpolator<double> * mBetaInt;
	spline::LinearInterpolator<double> * mGammaInt;
	Mat4 * mOrients;

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
