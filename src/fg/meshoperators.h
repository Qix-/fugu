#ifndef FG_MESHOPERATORS_H
#define FG_MESHOPERATORS_H

/**
 * Some standard mesh operators.
 *
 */

#include "fg/mesh.h"

namespace fg {

	/**
	 * \brief Extrudes all the faces around a vertex by a specified
	 * distance (along the vertex normal).
	 *
	 * This command selects all faces adjacent to a vertex and extrudes
	 * them along the vertex normal.
	 *
	 * @param v
	 * @param distance
	 */
	void extrude(Mesh* m, VertexProxy v, double distance);

	void extrude(Mesh* m, VertexProxy v, int width, Vec3 direction, double length, double expand);

}

#endif
