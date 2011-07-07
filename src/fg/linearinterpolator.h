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
    /**
	 * \brief Constructs a linear interpolator for the given points.
	 */
    LinearInterpolator(const std::vector<T> &mControlPoints);
	/**
	 * \brief Constructs an invalid interpolator.
	 */
    LinearInterpolator();

	LinearInterpolator(const LinearInterpolator<T> &other);

    LinearInterpolator<T>& operator=(const LinearInterpolator<T> &other);

    virtual T getPosition(double t) const;

    virtual T getDerivative(double t) const;

    virtual T getSecondDerivative(double t) const;

    virtual T *getApprox(int &n) const;
    virtual std::vector<T> getApproxVector(int &n) const;

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
