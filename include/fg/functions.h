#ifndef FG_FUNCTIONS_H
#define FG_FUNCTIONS_H

/**
 * Useful mathematical and geometric functions.
 * NB: Implementation is in functions.inc.
 */

namespace fg {
	template <typename T> T min(T a, T b);
	template <typename T> T lerp(T a, T b, T t);

	// step
	// pulse
	// clamp
 	// etc
}

#include "functions.inc"

#endif
