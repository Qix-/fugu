/**
 * \file
 * \brief This header contains the VCG implementation details of a Mesh.
 * \author ben
 * 
 * It is only needed if you are doing low vcg-level manipulations of a mesh.
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

#ifndef FG_MESHIMPL_H
#define FG_MESHIMPL_H

#include <vector>
#include "fg/vec3.h"
#include "fg/mesh.h"
#include "fg/fg.h"
#include "fg/armature.h"

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/topology.h> // NB: Needed this for Refine!
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/complex/complex.h>

namespace fg {
	/** Mesh Implementation is via VCG **/
	class VertexImpl;

	class FaceImpl;
	struct MyUsedTypes : public vcg::UsedTypes<
		vcg::Use<VertexImpl>::AsVertexType,
		vcg::Use<FaceImpl>::AsFaceType>{};
	class VertexImpl: public vcg::Vertex<
		MyUsedTypes,
		vcg::vertex::Coord3d,
		vcg::vertex::Color4b,
		vcg::vertex::Normal3d,
		vcg::vertex::BitFlags,
		vcg::vertex::VFAdj,
		vcg::vertex::InfoOcf,
		vcg::vertex::Mark> {

		public:
		VertexImpl():mNumBones(0){}

		// BONES!
		int getNumBones() const {return mNumBones;}
		int bindBone(BoneRef b, double weight){} ///< @return number of bones

	protected:
		int mNumBones;
		BoneWeakRef mBones[MAX_BONES_PER_VERTEX];
		double mBoneWeights[MAX_BONES_PER_VERTEX];
	};

	class FaceImpl: public vcg::Face<
		MyUsedTypes,
		vcg::face::FFAdj,
		vcg::face::VFAdj,
		vcg::face::Mark,
		vcg::face::VertexRef,
		vcg::face::Normal3d,
		// vcg::face::Color4b,
		vcg::face::BitFlags,
		//vcg::face::Normal3d,
		vcg::face::InfoOcf> {};
	class MeshImpl: public vcg::tri::TriMesh< std::vector< VertexImpl>, std::vector< FaceImpl > > {};

	// Implementations of mesh that use float (used for importing/exporting/...)
	class _FloatVertexImpl;
	class _FloatFaceImpl;
	struct _FloatMyUsedTypes : public vcg::UsedTypes<
		vcg::Use<_FloatVertexImpl>::AsVertexType,
		vcg::Use<_FloatFaceImpl>::AsFaceType>{};
	class _FloatVertexImpl: public vcg::Vertex<
			_FloatMyUsedTypes,
			vcg::vertex::Coord3f,
			vcg::vertex::Color4b,
			vcg::vertex::Normal3f,
			vcg::vertex::BitFlags,
			vcg::vertex::VFAdj,
			vcg::vertex::InfoOcf,
			vcg::vertex::Mark> {};
	class _FloatFaceImpl: public vcg::Face<
		_FloatMyUsedTypes,
		vcg::face::FFAdj,
		vcg::face::VFAdj,
		vcg::face::Mark,
		vcg::face::VertexRef,
		//vcg::face::Color4b,
		vcg::face::BitFlags,
		vcg::face::Normal3d,
		vcg::face::InfoOcf> {};
	class _FloatMeshImpl: public vcg::tri::TriMesh< std::vector< _FloatVertexImpl>, std::vector< _FloatFaceImpl > > {};

	void _copyFloatMeshIntoMesh(_FloatMeshImpl& fm, MeshImpl& m);
}

#endif
