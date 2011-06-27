#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

/**
 * Useful mathematical and geometric functions.
 * NB: Implementation is in functions.inc.
 */

namespace fg {
	template <typename T> T min(T a, T b);
	template <typename T> T lerp(T a, T b, T t);

	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);

	double random(); ///< Returns a random number between 0 and 1 inclusive
	double random(double low, double high); ///< Returns a random number between low and high inclusive

	// step
	// pulse
	// clamp
 	// etc
}

#include "functions.inc"

#endif
