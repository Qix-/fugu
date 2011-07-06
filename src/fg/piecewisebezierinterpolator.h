#ifndef FG_SPLINE_PIECEWISE_BEZIER_H
#define FG_SPLINE_PIECEWISE_BEZIER_H

#include <vector>
#include <utility>

#include "fg/BezierInterpolator.h"
#include "fg/linearinterpolator.h"

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
	 * \param gradients An array of length (n-1). The gradient at each end of each segment
	 *
	 */
    PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints, const std::pair<T,T> *gradients);

    /**
     * \brief Constructs a piecewise Bezier interpolator with inferred gradient
     * @param numControlPoints
     * @param controlPoints
     */
    PiecewiseBezierInterpolator(int numControlPoints, const T *controlPoints);

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
    virtual void setControlPoints(int numControlPoints, const T *controlPoints, const std::pair<T, T> *gradients);

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
	/**
	 * Gets the control points that define the segment/
	 */
	virtual const T * getSegmentControlPoints(int seg) const;

    virtual int getSegment(double t) const;
protected:
    virtual void deleteData();

    BezierInterpolator<T> **mSegInterpolators;
	std::pair<T,T> *mGradients;
};

	}}

#include "piecewiseBezierInterpolator.inl"
#endif
