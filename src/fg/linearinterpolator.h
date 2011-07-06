#ifndef FG_SPLINE_LINEAR_INTERPOLATOR_H
#define FG_SPLINE_LINEAR_INTERPOLATOR_H

#include <vector>
#include <utility>

#include "fg.h"
#include "interpolator.h"

namespace fg{
	namespace spline {
/**
 * \brief A class for perfoming linear interpolation.
 */
template<class T> class LinearInterpolator : public Interpolator< T >{
public:
    LinearInterpolator(int numControlPoints, const T *mControlPoints);

    /**
     * Gets the position along the curve for parameter value t.
     */
    virtual T getPosition(double t) const;

    virtual T getDerivative(double t) const;

    virtual T getSecondDerivative(double t) const;

    /**
     * Returns an array of points to approximated the curve with striaght
     * segments.
     *
     * \param n The suggested number of segments. If the interpolator creates
     *          a different number of segments this value is updated accordingly.
     *
     * @return An array of points of length n + 1.
     */
    virtual T *getApprox(int &n) const;

	virtual void getDomain(double &min, double &max) const;

	virtual int getNumSegments() const;

private:
    /**
     * Gets the internal domain of the parameter.
     *
     * \param min Gets set to the minimum value of the parameter.
     * \param max Gets set to the maximum value of the parameter.
     */
    void getInternalDomain(double &min, double &max) const;
    int getControlPointIndex(double t) const;
    double getInternalT(double externalT) const;
};

}
}

#include "fg/linearinterpolator.inl"
#endif
