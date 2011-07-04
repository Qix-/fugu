#ifndef FG_SPLINE_PIECEWISE_BEZIER_H
#define FG_SPLINE_PIECEWISE_BEZIER_H

#include <vector>

#include "BezierInterpolator.h"

namespace fg {
	namespace spline {
/**
 * \brief Interpolation of piecewise Bezier splines.
 */
template < class T > class PiecewiseBezierInterpolator : public Interpolator<T> {
public:
    /**
	 * \brief Constructs a peicewise Bezier interpolator.
	 *
	 * \param numControlPoints The number of control points.
	 * \param controlPoints An array of length n. The curve will pass through these points.
	 * \param gradients An array of length n. The gradient at each control point.
	 *
	 */
    PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints, const T *gradients);

    ~PiecewiseBezierInterpolator();

    virtual T getPosition (double t) const;
    virtual T getDerivative (double t) const;
    virtual T getSecondDerivative (double t) const;

    /**
     * Gets the domain of the parameter.
     *
     * \param min Gets set to the minimum value of the parameter.
     * \param max Gets set to the maximum value of the parameter.
     */
    void getDomain(double &min, double &max) const;

    /**
     * Sets the control points that will be interpolated between.
     */
    virtual void setControlPoints(int numControlPoints, const T *controlPoints, const T *gradients);

    /**
     * Sets the control points that will be interpolated between.
     */
    virtual void setControlPoints(int numControlPoints, const T *controlPoints);

    /**
     * Sets the given control point to the one given.
     */
    virtual void setControlPoint(int index, const T &cp, const T &grad);
    /**
     * Sets the given control point to the one given.
     */
    virtual void setControlPoint(int index, const T &cp);
	/**
	 * Get the number of cubic Bezier segments.
	 */
	virtual int getNumSegments() const;

protected:
    virtual void deleteData();
    virtual int getSegment(double t) const;

    BezierInterpolator<T> **mSegInterpolators;
	T *mGradients;
};

	}}

#include "piecewiseBezierInterpolator.inc"
#endif
