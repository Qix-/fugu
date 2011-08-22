/**
 * \file A bunch of utils that might help when building fg apps
 * \author ben
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

#ifndef FG_UTIL_H
#define FG_UTIL_H

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/ref.hpp>
#include <boost/tuple/tuple.hpp>
#include <utility>

/* Define foreach. (With workaround for eclipse cdt parser.) */
#ifdef __CDT_PARSER__
    #define foreach(a, b) for(a : b)
#else
    #define foreach(a, b) BOOST_FOREACH(a, b)
#endif

using boost::shared_ptr;
using boost::weak_ptr;
using boost::tuple;
using boost::make_tuple;

using std::pair;
using std::make_pair;

#endif
