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
    PiecewiseBezierInterpolator(const std::vector<T> &controlPoints, const std::vector< std::pair<T,T> > &gradients);

    /**
     * \brief Constructs a piecewise Bezier interpolator with inferred gradient
     * @param numControlPoints
     * @param controlPoints
     */
    PiecewiseBezierInterpolator(const std::vector<T> &controlPoints);
    PiecewiseBezierInterpolator();
	PiecewiseBezierInterpolator(const PiecewiseBezierInterpolator<T> &other);

    PiecewiseBezierInterpolator<T>& operator=(const PiecewiseBezierInterpolator<T> &other);


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
    virtual void setControlPoints(const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients);

    /**
     * Sets the control points that will be interpolated between.
     */
    virtual void setControlPoints(const std::vector<T> &controlPoints);

    /**
     * Sets the given control point to the one given.
     */
    virtual void setControlPoint(int index, const T &cp, const std::pair<T, T> &grad);
	
    /**
     * Sets the given control point to the one given.
     */
    virtual void setControlPoint(int index, const T &cp);

	virtual void appendControlPoint(const T &cp);
	virtual void appendControlPoint(const T &cp, const std::pair<T,T> &g);

    virtual std::vector<T> getControlPoints() const;
    virtual T getControlPoint(int i) const;
	/**
	 * Get the number of cubic Bezier segments.
	 */
	virtual int getNumSegments() const;
	virtual int getNumControlPoints() const;
	/**
	 * Gets the control points that define the segment/
	 */
	virtual std::vector<T> getSegmentControlPoints(int seg) const;

    virtual int getSegment(double t) const;
protected:
    virtual void deleteData();
    virtual void smoothGradients();

    std::vector< BezierInterpolator<T> > mSegInterpolators;
	int mNumControlPoints;
	T mFirstControlPoint;
};
}}

#include "piecewiseBezierInterpolator.inl"
#endif
