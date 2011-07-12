#ifndef FG_GC_CARRIER_CURVE_LINEAR_H
#define FG_GC_CARRIER_CURVE_LINEAR_H

#include "fg/fg.h"
#include "fg/carriercurve.h"
#include "fg/linearinterpolator.h"
#include "fg/quat.h"

#include <vector>
#include <utility>

namespace fg {
	namespace gc {
class CarrierCurveLinear : public CarrierCurve
{
public:
    CarrierCurveLinear(const std::vector<Mat4> &refFrames);
	CarrierCurveLinear(const CarrierCurveLinear &other);
	CarrierCurveLinear & operator=(const CarrierCurveLinear &other);
	~CarrierCurveLinear();

    virtual Quat orient(double v) const;
	virtual Quat orientDer(double v) const;

	virtual std::pair<Quat, Quat> getSegOrients(int seg) const;

	virtual const spline::LinearInterpolator<Vec3> * getInterpolator() const;

    virtual void setReferenceFrames(const std::vector<Mat4> &refFrames);
	virtual std::vector<Mat4> getReferenceFrames() const;
private:
//    virtual void getOrientation(double t, Vec3 *H, Vec3 *U, Vec3 *L) const;
	void deleteData();

	spline::LinearInterpolator<Vec3> * mInterpolator;
	std::vector<Mat4> mRFrames;
};
}
}

#endif
