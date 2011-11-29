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

#ifndef FG_PHYLO_H
#define FG_PHYLO_H

namespace fg {
    
    // functions for calculating phyllotaxis on oblate and prolate spheroids
    double getEllipseArea(double a, double c, double h);
    double findEllipseH(double a, double c, double area);
    double getAngleFromH(double a, double c, double h);

}
#endif
