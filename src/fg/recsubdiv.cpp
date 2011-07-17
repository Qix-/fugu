/**
 * \file
 * \brief Defines fg::spline::RecSubDiv
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

#include <iostream>

#include "fg/recsubdiv.h"

namespace fg {
    namespace spline {

        const double curve_distance_epsilon                  = 1e-30;
        const double curve_collinearity_epsilon              = 1e-30;
        const double curve_angle_tolerance_epsilon           = 0.01;
        const int    curve_recursion_limit                   = 32;

        void RecSubDiv::init( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4 )
        {
            m_points.clear();
            m_distance_tolerance_square = 0.5 / m_approximation_scale;
            m_distance_tolerance_square *= m_distance_tolerance_square;
            bezier( p1, p2, p3, p4 );
            m_count = 0;
        }

        void RecSubDiv::recursive_bezier( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4, unsigned level )
        {
            // Calculate all the mid-points of the line segments
            //----------------------
            Vec3 p12   = ( p1 + p2 ) * .5;
            Vec3 p23   = ( p2 + p3 ) * .5;
            Vec3 p34   = ( p3 + p4 ) * .5;
            Vec3 p123  = ( p12 + p23 ) * .5;
            Vec3 p234  = ( p23 + p34 ) * .5;
            Vec3 p1234 = ( p123 + p234 ) * .5;
            // Try to approximate the full cubic curve by a single straight line
            //------------------
            Vec3 dp = p4 - p1;
            double d2 = ( ( p2 - p1 ).cross( p2 - p4 ).length() ); ///((p4-p1).length());//fabs(((x2 - x4) * dy - (y2 - y4) * dx));
            double d3 = ( ( p3 - p1 ).cross( p3 - p4 ).length() ); ///((p4-p1).length());//fabs(((x3 - x4) * dy - (y3 - y4) * dx));

            // If the curvature doesn't exceed the distance_tolerance value
            // we tend to finish subdivisions.
            //---------------------
            if( ( d2 + d3 ) * ( d2 + d3 ) < m_distance_tolerance_square * dp.dot( dp ) )
            {
                m_points.push_back( p1234 );
                return;
            }

            //if( m_angle_tolerance < curve_angle_tolerance_epsilon )
            //{
			//	std::cout << "Broke angle tol\n";
            //    m_points.push_back( p1234 );
            //    return;
            //}

            // Angle & Cusp Condition
            //----------------------
            double da1 = acos( ( p2 - p1 ).dot( p3 - p2 ) / ( ( p2 - p1 ).length() * ( p3 - p2 ).length() ) ); //fabs(a23 - atan2(y2 - y1, x2 - x1));
            double da2 = acos( ( p3 - p2 ).dot( p4 - p3 ) / ( ( p3 - p2 ).length() * ( p4 - p3 ).length() ) ); //fabs(a23 - atan2(y2 - y1, x2 - x1));

            if( da1 >= M_PI ) da1 = 2.*M_PI - da1;

            if( da2 >= M_PI ) da2 = 2.*M_PI - da2;

            //std::cout << da1 << ", " << da2 << std::endl;
            //std::cout << da1 + da2 << std::endl;

            if( da1 + da2 < m_angle_tolerance )
            {
                // Finally we can stop the recursion
                //----------------------
                m_points.push_back( p1234 );
                return;
            }

            // Continue subdivision
            //----------------------
            recursive_bezier( p1, p12, p123, p1234, level + 1 );
            recursive_bezier( p1234, p234, p34, p4, level + 1 );
        }

//void RecSubDiv::recursive_bezier( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4,
//                                  unsigned level)
//{
//    if(level > curve_recursion_limit)
//    {
//        return;
//    }
//
//    // Calculate all the mid-points of the line segments
//    //----------------------
//    Vec3 p12   = (p1 + p2) * .5;
//    Vec3 p23   = (p2 + p3) * .5;
//    Vec3 p34   = (p3 + p4) * .5;
//    Vec3 p123  = (p12 + p23) * .5;
//    Vec3 p234  = (p23 + p34) * .5;
//    Vec3 p1234 = (p123 + p234) * .5;
//
//
//
//    // Try to approximate the full cubic curve by a single straight line
//    //------------------
//    Vec3 dp = p4-p1;
//
//    // Distance between p2, p3 and the vector (p4-p1)
//    //------------------
//    double d2 = ((p2-p1).cross(p2-p4).length())/((p4-p1).length());//fabs(((x2 - x4) * dy - (y2 - y4) * dx));
//    double d3 = ((p3-p1).cross(p3-p4).length())/((p4-p1).length());//fabs(((x3 - x4) * dy - (y3 - y4) * dx));
//
//  double da1, da2, k;
//    switch((int(d2 > curve_collinearity_epsilon) << 1) +
//            int(d3 > curve_collinearity_epsilon))
//    {
//    case 0:
//        // All collinear OR p1==p4
//        //----------------------
//        k = dp.dot( dp );// dx*dx + dy*dy;
//        if(k < m_distance_tolerance_square)
//        {
//            d2 = p1.dot( p2 );//calc_sq_distance(x1, y1, x2, y2);
//            d3 = p4.dot( p3 );//calc_sq_distance(x4, y4, x3, y3);
//        }
//        else
//        {
//          return;
//            k   = 1 / k;
//          da  = p2 - p1;
//            d2  = k * da.dot( dp );//(da1*dx + da2*dy);
//          da  = p3 - p1;
//            d3  = k * da.dot( dp );//(da1*dx + da2*dy);
//            if(d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
//            {
//                // Simple collinear case, 1---2---3---4
//                // We can leave just two endpoints
//                return;
//            }
//            if(d2 <= 0) d2 = p2.dot( p1 );//calc_sq_distance(x2, y2, x1, y1);
//            else if(d2 >= 1) d2 = p2.dot( p4 );//calc_sq_distance(x2, y2, x4, y4);
//            else             d2 = p2.dot( p1 + dp * d2 );//calc_sq_distance(x2, y2, x1 + d2*dx, y1 + d2*dy);
//
//            if(d3 <= 0) d3 = p3.dot( p1 );//calc_sq_distance(x3, y3, x1, y1);
//            else if(d3 >= 1) d3 = p3.dot( p4 );//calc_sq_distance(x3, y3, x4, y4);
//            else             d3 = p3.dot( p1 + dp * d3 );//calc_sq_distance(x3, y3, x1 + d3*dx, y1 + d3*dy);
//        }
//        if(d2 > d3)
//        {
//            if(d2 < m_distance_tolerance_square)
//            {
//                m_points.push_back( p2 );
//                return;
//            }
//        }
//        else
//        {
//            if(d3 < m_distance_tolerance_square)
//            {
//                m_points.push_back( p3 );
//                return;
//            }
//        }
//        break;
//
//    case 1:
//        // p1,p2,p4 are collinear, p3 is significant
//        //----------------------
//        if(d3 * d3 <= m_distance_tolerance_square * dp.dot( dp ))//(dx*dx + dy*dy))
//        {
//            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
//            {
//                m_points.push_back( p23 );
//                return;
//            }
//
//            // Angle Condition
//            //----------------------
//          /*
//            float da1 = fabs(atan2(p4.getY() - p3.getY(), p4.getX() - p3.getX()) -
//                           atan2(p3.getY() - p2.getY(), p3.getX() - p2.getX()));
//            if(da1 >= M_PI) da1 = 2.*M_PI - da1;
//
//            if(da1 < m_angle_tolerance)
//            {
//                m_points.push_back( p2 );
//                m_points.push_back( p3 );
//                return;
//            }
//
//            if(m_cusp_limit != 0.0)
//            {
//                if(da1 > m_cusp_limit)
//                {
//                    m_points.push_back( p3 );
//                    return;
//                }
//            }
//          */
//        }
//        break;
//
//    case 2:
//        // p1,p3,p4 are collinear, p2 is significant
//        //----------------------
//        if(d2 * d2 <= m_distance_tolerance_square * dp.dot(dp))//(dx*dx + dy*dy))
//        {
//            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
//            {
//                m_points.push_back( p23 );//x23, y23));
//                return;
//            }
//
//            // Angle Condition
//            //----------------------
//          /*
//            float da1 = fabs(atan2(p3.getY() - p2.getY(), p3.getX() - p2.getX()) -
//                           atan2(p2.getY() - p1.getY(), p2.getX() - p1.getX()));
//            if(da1 >= M_PI) da1 = 2.*M_PI - da1;
//
//            if(da1 < m_angle_tolerance)
//            {
//                m_points.push_back( p2 );
//                m_points.push_back( p3 );
//                return;
//            }
//
//            if(m_cusp_limit != 0.0)
//            {
//                if(da1 > m_cusp_limit)
//                {
//                    m_points.push_back( p2 );
//                    return;
//                }
//            }
//          */
//        }
//        break;
//
//    case 3:
//        // Regular case
//        //-----------------
//        if((d2 + d3)*(d2 + d3) <= m_distance_tolerance_square * dp.dot(dp))//(dx*dx + dy*dy))
//        {
//            // If the curvature doesn't exceed the distance_tolerance value
//            // we tend to finish subdivisions.
//            //----------------------
//            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
//            {
//                m_points.push_back( p23 );
//                return;
//            }
//
//            // Angle & Cusp Condition
//            //----------------------
//          /*
//            k   = atan2(y3 - y2, x3 - x2);
//            k   = atan2(p3.getY() - p2.getY(), p3.getX() - p2.getX());
//            float da1 = fabs(k - atan2(y2 - y1, x2 - x1));
//            da2 = fabs(atan2(y4 - y3, x4 - x3) - k);
//            if(da1 >= pi) da1 = 2*pi - da1;
//            if(da2 >= pi) da2 = 2*pi - da2;
//
//            if(da1 + da2 < m_angle_tolerance)
//            {
//                // Finally we can stop the recursion
//                //----------------------
//                m_points.add(point_d(x23, y23));
//                return;
//            }
//
//            if(m_cusp_limit != 0.0)
//            {
//                if(da1 > m_cusp_limit)
//                {
//                    m_points.add(point_d(x2, y2));
//                    return;
//                }
//
//                if(da2 > m_cusp_limit)
//                {
//                    m_points.add(point_d(x3, y3));
//                    return;
//                }
//            }
//          */
//        }
//        break;
//    }
//
//    // Continue subdivision
//    //----------------------
//    recursive_bezier(p1, p12, p123, p1234, level + 1);
//    recursive_bezier(p1234, p234, p34, p4, level + 1);
//}

//------------------------------------------------------------------------
        void RecSubDiv::bezier( const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4 )
        {
            m_points.push_back( p1 );
            recursive_bezier( p1, p2, p3, p4, 0 );
            m_points.push_back( p4 );
        }

    }
}
