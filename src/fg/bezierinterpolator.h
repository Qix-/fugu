#ifndef FG_SPLINE_BEZIER_INTERPOLATOR_H
#define FG_SPLINE_BEZIER_INTERPOLATOR_H

#include "interpolator.h"

namespace fg {
namespace spline {

/**
 * \brief Performs Bezier interpolation.
 */
template<class T>
class BezierInterpolator : public Interpolator<T> {
public:
    BezierInterpolator (int degree, T* controlPoints);
    virtual ~BezierInterpolator ();

    virtual int getDegree () const;
	virtual int getNumSegments () const;

    virtual void setControlPoints (int degree, T* controlPoints);

    virtual T getPosition (double t) const;
    virtual T getDerivative (double t) const;
    virtual T getSecondDerivative (double t) const;

    virtual void getDomain(double &min, double &max) const
    {
        min = 0.f;
        max = 1.f;
    }

protected:
    int mDegree;
    int mNumControlPoints;
    T* mDer1ControlPoint;
    T* mDer2ControlPoint;
    double** mChoose;

    virtual void deleteData ();
};

}
}

#include "bezierInterpolator.inl"

#endif
