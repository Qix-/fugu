/**
 * \file
 * \brief Defines operators which act on a fg::Mesh
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

#ifndef FG_MESHOPERATORS_H
#define FG_MESHOPERATORS_H

/**
 * \defgroup meshops Mesh Operators
 */

#include "fg/mesh.h"
#include "fg/vertex.h"
#include "fg/pos.h"

#include <list>

#include <boost/shared_ptr.hpp>

namespace fg {

	/**
	 * \brief Extrudes all the faces around a vertex by a specified
	 * distance (along the vertex normal).
	 *
	 * \ingroup meshops
	 */
	void extrude(Mesh* m, VertexProxy v, double distance);

	/**
	 * \brief Extended version of extrude
	 *
	 * \deprecated Use expand-less version of extrude instead
	 *
	 * \param m The mesh
	 * \param v The vertex to extrude around
	 * \param width Width of extrusion (specifies the radius of a fan of faces surrounding v)
	 * \param direction The direction to extrude
	 * \param length The distance to extrude
	 * \param expand The expansion applied to the extruded faces
	 *
	 * \ingroup meshops
	 */
	void extrude(Mesh* m, VertexProxy v, int width, Vec3 direction, double length, double expand);

	/**
	 * \brief Extrudes all faces within distance w of v. Note that the extruded faces will remain aligned as
	 * they were before. They have to be manually rotated if you want them to line up with the axis of extrusion.
	 *
	 * \param m The mesh
	 * \param v The vertex to extrude around
	 * \param w Width of extrusion (specifies the radius of a fan of faces surrounding v)
	 * \param direction The direction to extrude
	 * \param magnitude The distance to extrude
	 *
	 * \ingroup meshops
	 */
	void extrude(Mesh* m, VertexProxy v, int width, Vec3 direction, double magnitude);


	/**
	 * \brief Get all vertices (unordered) lying a distance n (in edges) surrounding a vertex.
	 *
	 * \ingroup meshops
	 */
	boost::shared_ptr<Mesh::VertexSet> getVerticesAtDistance(Mesh* m, VertexProxy v, int n);

	/**
	 * \brief Get all vertices lying at distance n or less (in edges) surrounding a vertex.
	 *
	 * \ingroup meshops
	 */
	boost::shared_ptr<Mesh::VertexSet> getVerticesWithinDistance(Mesh* m, VertexProxy v, int n);

	/**
	 * \brief Get the edge loop at distance n surrounding vertex v
	 * \ingroup meshops
	 */
	boost::shared_ptr<Mesh::VertexSet> nloop(Mesh* m, VertexProxy v, int n);

	/**
	 * \brief Split the edge pointed to by Pos p
	 * \ingroup meshops
	 */
	void splitEdge(Mesh* m, Pos p);
}

#endif
