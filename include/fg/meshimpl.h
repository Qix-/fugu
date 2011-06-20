#ifndef FG_MESHIMPL_H
#define FG_MESHIMPL_H

/**
 * This header contains the VCG implementation details of a Mesh.
 * It only needs to be #included in files which do low vcg-level
 * manipulations of a mesh.
 */

#include <vector>
#include "fg/vec3.h"
#include "fg/mesh.h"

#include <vcg/simplex/vertex/base.h>
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
		vcg::vertex::Normal3d,
		vcg::vertex::BitFlags,
		vcg::vertex::VFAdj,
		vcg::vertex::InfoOcf,
		vcg::vertex::Mark>, public Vertex {
		// Adapt the interface
		public:
		virtual Vec3& pos(); // {return static_cast<Vec3&>(P());}
	};
	class FaceImpl: public vcg::Face<
		MyUsedTypes,
		vcg::face::FFAdj,
		vcg::face::VFAdj,
		vcg::face::Mark,
		vcg::face::VertexRef,
		vcg::face::Normal3d,
		vcg::face::Color4b,
		vcg::face::BitFlags,
		vcg::face::Normal3d,
		vcg::face::InfoOcf> {};
	class MeshImpl: public vcg::tri::TriMesh< std::vector< VertexImpl>, std::vector< FaceImpl > > {};
}

#endif
