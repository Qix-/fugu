/**
 * \file
 * \brief A Pos is a (vertex,edge,face) tuple, which can be used to navigate a mesh.
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
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#ifndef FG_POS_H
#define FG_POS_H

#include <ostream>

#include <vcg/simplex/face/pos.h>

#include "fg/mesh.h"
#include "fg/meshimpl.h"
#include "fg/util.h"

namespace fg {
	/** \brief A Pos is a (vertex,edge,face) tuple, which can be used to navigate a mesh.
	 *
	 * It is a wrapper around vcg::Pos, which implements the cell-tuple data structure.
	 */
	class Pos {
	public:
		Pos( shared_ptr<FaceProxy> fp, int edge, shared_ptr<VertexProxy> vp);
		Pos( shared_ptr<FaceProxy> fp, shared_ptr<VertexProxy> vp);
		Pos( const Pos& p );

		void flipV();
		void flipE();
		void flipF();

		shared_ptr<VertexProxy> getV();
		int getE();
		shared_ptr<FaceProxy> getF();

		bool operator==(const Pos& p) const;
	private:
		typedef vcg::face::Pos<fg::FaceImpl> VCGPos;
		VCGPos mPos;
		Mesh* mMesh;
	};
}

std::ostream& operator<<(std::ostream& o, const fg::Pos& p);

#endif
