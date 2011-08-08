/**
 * \file
 * \brief Declares fg::gc::RecSubDiv
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

#ifndef FG_SPLINE_REC_SUB_DIV_H
#define FG_SPLINE_REC_SUB_DIV_H

#include <vector>

#include "fg/fg.h"

namespace fg {
    namespace gc {

        /**
         * \brief Performs recursive subdivision on a bezier curve.
         */
        class RecSubDiv {
            public:
                RecSubDiv() :
                    m_approximation_scale( 1.0 ),
                    m_angle_tolerance( 0.0 ),
                    m_cusp_limit( 0.0 ),
                    m_count( 0 )
                {}

                RecSubDiv( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4 ) :
                    m_approximation_scale( 1.0 ),
                    m_angle_tolerance( 0.0 ),
                    m_cusp_limit( 0.0 ),
                    m_count( 0 )
                {
                    init( p1, p2, p3, p4 );
                }
                RecSubDiv( const std::vector<Vec3> &points ):
                    m_approximation_scale( 1.0 ),
                    m_angle_tolerance( 0.0 ),
                    m_cusp_limit( 0.0 ),
                    m_count( 0 )
                {
                    init( points[0], points[1], points[2], points[3] );
                }

                void init( const std::vector<Vec3> &points )
                {
                    init( points[0], points[1], points[2], points[3] );
                }
                void init( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4 );
                void reset() {
                    m_points.clear();
                    m_count = 0;
                }

                void approximation_scale( double s ) {
                    m_approximation_scale = s;
                }
                double approximation_scale() const {
                    return m_approximation_scale;
                }

                void angle_tolerance( double a ) {
                    m_angle_tolerance = a;
                }
                double angle_tolerance() const {
                    return m_angle_tolerance;
                }

                void cusp_limit( double v )
                {
                    m_cusp_limit = ( v == 0.0 ) ? 0.0 : M_PI - v;
                }

                double cusp_limit() const
                {
                    return ( m_cusp_limit == 0.0 ) ? 0.0 : M_PI - m_cusp_limit;
                }

                const std::vector<Vec3> get_data() const
                {
                    return m_points;
                }

            protected:
                double               m_approximation_scale;
                double               m_distance_tolerance_square;
                double               m_angle_tolerance;
                double               m_cusp_limit;
                unsigned             m_count;
                std::vector<Vec3>    m_points;

                void bezier( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4 );

                void recursive_bezier( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4,
                                       unsigned level );
        };

    }
}

#endif
