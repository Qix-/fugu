/**
 * \file
 * \brief Declares fg::spline::PBezInterp
 * \author james
 *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#ifndef FG_SPLINE_PIECEWISE_BEZIER_H
#define FG_SPLINE_PIECEWISE_BEZIER_H

#include <vector>
#include <utility>

#include "fg/bezinterp.h"

namespace fg {
    namespace spline {
        /**
         * \brief Interpolation of piecewise Bezier splines.
         */
        template < class T > class PBezInterp : public Interpolator<T> {
            public:

                /**
                 * \brief Constructs a peicewise Bezier interpolator.
                 *
                 * \param controlPoints An array of length n. The curve will pass through these points.
                 * \param gradients An array of length (n-1). The gradient at each end of each segment
                 *
                 */
                PBezInterp( const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients );

                /**
                 * \brief Constructs a piecewise Bezier interpolator with inferred gradient
                 * @param controlPoints
                 */
                PBezInterp( const std::vector<T> &controlPoints );
                PBezInterp();
                PBezInterp( const PBezInterp<T> &other );

                PBezInterp<T>& operator=( const PBezInterp<T> &other );


                ~PBezInterp();

                virtual T getPosition( double t ) const;
                virtual T getDerivative( double t ) const;
                virtual T getSecondDerivative( double t ) const;

                /**
                 * Gets the domain of the parameter.
                 *
                 * \param min Gets set to the minimum value of the parameter.
                 * \param max Gets set to the maximum value of the parameter.
                 */
                void getDomain( double &min, double &max ) const;

                /**
                 * Sets the control points that will be interpolated between.
                 */
                virtual void setControlPoints( const std::vector<T> &controlPoints, const std::vector< std::pair<T, T> > &gradients );

                /**
                 * Sets the control points that will be interpolated between.
                 */
                virtual void setControlPoints( const std::vector<T> &controlPoints );

                /**
                 * Sets the given control point to the one given.
                 */
                virtual void setControlPoint( int index, const T &cp, const std::pair<T, T> &grad );

                /**
                 * Sets the given control point to the one given.
                 */
                virtual void setControlPoint( int index, const T &cp );

                virtual void appendControlPoint( const T &cp );
                virtual void appendControlPoint( const T &cp, const std::pair<T, T> &g );

                virtual std::vector<T> getControlPoints() const;
                virtual T getControlPoint( int i ) const;
                /**
                 * Get the number of cubic Bezier segments.
                 */
                virtual int getNumSegments() const;
                virtual int getNumControlPoints() const;
                /**
                 * Gets the control points that define the segment/
                 */
                virtual std::vector<T> getSegmentControlPoints( int seg ) const;

                virtual int getSegment( double t ) const;
            protected:
                virtual void deleteData();
                virtual void smoothGradients();
				virtual const BezInterp<T> getSegmentInterpolator( int seg ) const;

                std::vector< BezInterp<T> > mSegInterpolators;
				BezInterp<T> mClosingInterp;
				std::vector< std::pair< Vec3, Vec3 > > mGradients;
                int mNumControlPoints;
                T mFirstControlPoint;
        };
    }
}

#include "fg/pbezinterp.inl"
#endif
