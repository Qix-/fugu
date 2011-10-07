/**
 * \file
 * \author jonmc
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

#include "fg/phyllo.h"
#include <cmath>
#include <iostream>

namespace fg {

    using namespace std;    
    /**
     * getEllipseArea
     *
     * returns the area of a spherical ellipsoid with equatorial radius a
     * and polar radius c, defined from z=c to z=h, where c > z >= -c
     */
    double getEllipseArea(double a, double c, double h) {
        double aa = a * a;
        double cc = c * c;
        double c4 = c * c * c * c;
        double hh = h * h;
        double e, e2, sqc, res;
        
        if (c < a) {
            e2 = aa - cc;
            e = sqrt(e2);
            sqc = sqrt(c4 + e2*hh);
            return (a * M_PI*(a*e + h * sqrt(e2 + (e2*e2*hh)/c4) +
                              cc * log( ((a + e)*(e*h+sqc))/(cc * c) ) ))/e;
        } else {
            e2 = aa - cc;
            sqc = h * sqrt(1+((e2*hh)/c4));
            e = sqrt(fabs(e2));
            return a * M_PI * (a+sqc+(cc * (asin(e/c) + asin((e*h)/cc) ) / e));
        }
    }
    
    /**
     * findEllipseH
     *
     * returns an approximate value of h that matches the area supplied for an
     * spherical ellipsoid with equatorial radius a and polar radius c, defined
     * from z=b to z=h.
     * Uses a simple binary subdivision method which is not at all efficient.
     */
    double findEllipseH(double a, double c, double area) {
        double hMin, hMax, hMid, aMid;
        double maxQ = 0.001;
        
        /* basic error check */
        if (area < 0) return 0;
        if (area >= getEllipseArea(a,c,c)) return c;
        if (a == c) cerr << "Warning: findEllipseH: a == c (" << a << " == " << c <<")" << endl;
        
        hMin = 0;
        hMax = 2 * c;
        hMid = c;
        aMid = getEllipseArea(a,c,hMid-c);
        while (fabsf(aMid - area) > maxQ) {	
            if (aMid < area) {
                hMin = hMid;
                hMid = hMid + (hMax - hMid)/2.0;
            } else {
                hMax = hMid;
                hMid = hMin + (hMid - hMin)/2.0;
            }
            aMid = getEllipseArea(a,c,hMid-c);
        }
        return hMid-c;
    }
    
    /*
     * getAngleFromH
     *
     * returns angle for normal of ellipsoid(a,c) at z = h
     */
    double getAngleFromH(double a, double c, double h) {
        double res;
        res =   -(a * h)/((c*c)*sqrt(1.0 - ((h*h)/(c*c))) );
        return atan(res);
    }
    
    
    
}
