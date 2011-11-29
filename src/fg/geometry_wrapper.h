/*
 * fg Wrappers for the geometry routines.
 */

#ifndef FG_GEOMETRYWRAPPER_H
#define FG_GEOMETRYWRAPPER_H

#include "geometry.h"
#include "vec3.h"

namespace fg {
	inline double line_exp_point_dist_3d(Vec3& p1, Vec3& p2, Vec3& p){
		return ::line_exp_point_dist_3d(p1.V(),p2.V(),p.V());
	}

	inline double segment_point_dist_3d(Vec3& p1, Vec3& p2, Vec3& p){
		return ::segment_point_dist_3d(p1.V(),p2.V(),p.V());
	}
}

#endif
