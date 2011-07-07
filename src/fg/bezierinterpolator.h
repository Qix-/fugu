#ifndef FG_SPLINE_BEZIER_INTERPOLATOR_H
#define FG_SPLINE_BEZIER_INTERPOLATOR_H

#include <vector>

#include "interpolator.h"

namespace fg {
namespace spline {

/**
 * \brief Performs Bezier interpolation.
 */
template<class T>
class BezierInterpolator : public Interpolator<T> {
public:
    BezierInterpolator (const std::vector<T> &controlPoints);
    BezierInterpolator ();
	BezierInterpolator(const BezierInterpolator &other);

    BezierInterpolator<T>& operator=(const BezierInterpolator &other);
    virtual ~BezierInterpolator ();

    virtual int getDegree () const;
	virtual int getNumSegments () const;

    virtual void setControlPoints (const std::vector<T> &controlPoints);

	virtual void appendControlPoint (const T &cp);

    virtual T getPosition (double t) const;
    virtual T getDerivative (double t) const;
    virtual T getSecondDerivative (double t) const;

    virtual void getDomain(double &min, double &max) const
    {
        min = 0.f;
        max = 1.f;
    }

protected:
    std::vector<T> mDer1ControlPoint;
    std::vector<T> mDer2ControlPoint;
    double** mChoose;

    virtual void deleteData ();
	virtual void allocateChoose();
	virtual void deleteChoose();
};

}
}

#include "bezierInterpolator.inl"

#endif
