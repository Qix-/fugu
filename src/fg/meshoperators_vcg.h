/**
 * \file
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

#ifndef FG_MESHOPERATORS_VCG_H
#define FG_MESHOPERATORS_VCG_H



#include "fg/meshimpl.h"

#include <vector>
#include <set>

#include <vcg/space/point3.h>

namespace fg {
	/**
	 * \brief Operators which act directly on the vcg implementation of a mesh.
	 *
	 * NB: Vertex* is a vcg::Mesh::Vertex, not a fg::Vertex!
	 */
	class Extrude {
	public:
		typedef MeshImpl MyMesh;
		// typedef typename MyMesh::VertexType VertexType;
		typedef MyMesh::VertexType Vertex;
		typedef MyMesh::VertexPointer VertexPointer;
		typedef MyMesh::VertexIterator VertexIterator;
		typedef MyMesh::FaceType Face;
		typedef MyMesh::FacePointer FacePointer;
		typedef MyMesh::FaceIterator FaceIterator;
		typedef vcg::face::Pos<fg::FaceImpl> Pos;

		typedef std::vector<VertexPointer*> VPUpdateList;
		typedef std::vector<FacePointer*> FPUpdateList;

		/**
		 * Extrudes the set of triangles within distance w in the specified direction and magnitude
		 * @param m
		 * @param v
		 * @param vpul
		 * @param w
		 * @param direction
		 * @param magnitude
		 * @return
		 */
		static std::set<VertexPointer> extrude(MyMesh* m, Vertex*& v, VPUpdateList& vpul, FPUpdateList& fpul, int w, vcg::Point3d direction, double magnitude);

		/// Splits the edge specified by Pos
		static void splitEdge(MyMesh* m, Pos& p, VPUpdateList& vpul, FPUpdateList& fpul);

		static bool isEdgeLoop(std::vector<VertexPointer>& loop);

		/// \deprecated Use the simpler extrude instead
		static std::set<VertexPointer> extrude(MyMesh* m, Vertex*& v, VPUpdateList& vpul, FPUpdateList& fpul, int width, vcg::Point3d direction, double length, double expand);
	private:
		static void error(const char* msg);
	};
}

#endif
