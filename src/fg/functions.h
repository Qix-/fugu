#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

/**
 * Useful mathematical and geometric functions.
 */

#include <cfloat>

#include "fg/vec3.h"
#include "fg/mat4.h"

namespace fg {
	const double EPSILON = DBL_EPSILON; // A small double..

	template <typename T> T min(T a, T b);
	template <typename T> T lerp(T a, T b, T t);

	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

	double random(); ///< Returns a random number between 0 and 1 inclusive
	double random(double low, double high); ///< Returns a random number between low and high inclusive

	bool approx(double a, double b, double epsilon = EPSILON); ///< Checks if a~b (i.e., |a-b| < epsilon)
	bool approx(const Vec3& a, const Vec3& b, double epsilon = EPSILON);
	// step
	// pulse
	// clamp
 	// etc
}

#include "functions.inc"

#endif
