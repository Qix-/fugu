#ifndef FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H
#define FG_GC_CARRIER_CURVE_PIECEWISE_BEZIER_H

#include <vector>
#include <utility>

#include "fg/fg.h"
#include "fg/carriercurve.h"
#include "fg/piecewisebezierinterpolator.h"
#include "fg/linearinterpolator.h"

namespace fg {
	namespace gc {
class CarrierCurvePiecewiseBezier : public CarrierCurve
{
public:
    CarrierCurvePiecewiseBezier(const std::vector<Mat4> &refFrames, const std::vector< std::pair<double,double> > &stiffness);
    CarrierCurvePiecewiseBezier(const std::vector<Mat4> &refFrames);
    CarrierCurvePiecewiseBezier(const CarrierCurvePiecewiseBezier &other);
	CarrierCurvePiecewiseBezier& operator=(const CarrierCurvePiecewiseBezier &other);
	CarrierCurvePiecewiseBezier();
	~CarrierCurvePiecewiseBezier();

    virtual Quat orient(double v) const;
	virtual Quat orientDer(double v) const;

    virtual std::pair<Quat,Quat> getSegOrients(int seg) const;

	virtual const spline::PiecewiseBezierInterpolator<Vec3> * getInterpolator() const;

    virtual void setReferenceFrames(const std::vector<Mat4> &refFrames, const std::vector< std::pair<double,double> > &stiffness);
    virtual void setReferenceFrames(const std::vector<Mat4> &refFrames);
	virtual std::vector<Mat4> getReferenceFrames() const;
private:
    virtual Quat getFrenetFrame(double v) const;
    void updateInflectionPoints( int seg );
	void deleteData();

    std::vector<int>  mSegType;
	std::vector< std::pair <double, double> > mInflectionPoints;
	std::vector<Mat4> mRFrames;

	spline::PiecewiseBezierInterpolator<Vec3> * mInterpolator;

// These should go somewhere else!
public:
    static int findInflectionPointsVec( const std::vector<Vec3> &cp, double *i1, double *i2 );
	static double determinant(double a, double b, double c);
	static int rootsQuad(double a, double b, double det, double *r1, double *r2);
	static double rootLinear(double m, double c);
	static int roots(double a, double b, double c, double *r1, double *r2);
    static int roots( Vec3 a, Vec3 b, Vec3 c, double *r1, double *r2 );
	static bool collinear( const std::vector< Vec3 > &p );
	static bool parallel( const Vec3 &v1, const Vec3 &v2 );
};
	}
}

#endif
