#ifndef FG_H
#define FG_H

#include "fg/universe.h"
#include "fg/functions.h"
#include "fg/mesh.h"

namespace fg {
	// SYSTEM CONSTANTS

	/// Maximum number of bones that can be bound to each vertex. NB: increasing this increases memory usage
	static const int MAX_BONES_PER_VERTEX = 4;
}

#endif
