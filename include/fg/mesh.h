#ifndef FG_MESH_H
#define FG_MESH_H

/**
 * fg::Mesh is a triangular mesh.
 *
 * Implementation notes:
 * - It wraps a VCG mesh with as much info as we can jam in...
 *
 */

#include <ostream>
#include <vector>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/math/perlin_noise.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>


#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/complex/algorithms/refine.h>

#include <boost/shared_ptr.hpp>

// for opengl
#include "GL/glew.h"
#include <wrap/gl/trimesh.h>

namespace fg {

	class Mesh {
		public:

		// Mesh factories
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
		};

		~Mesh();

		// Accessors


		// Queries
		void getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz);

		// OpenGL helpers
		void drawGL();

		private:
		Mesh(); // can't create a mesh from nothing .. yet

		// VCG Implementation
		class MyFace;
		class MyVertex;

		struct MyUsedTypes : public vcg::UsedTypes<
			vcg::Use<MyVertex>::AsVertexType,
			vcg::Use<MyFace>::AsFaceType>{};

		class MyVertex: public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags, vcg::vertex::VFAdj, vcg::vertex::InfoOcf, vcg::vertex::Mark>{};
		class MyFace: public vcg::Face< MyUsedTypes, vcg::face::FFAdj, vcg::face::Mark, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::BitFlags, vcg::face::InfoOcf> {};
		class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

		MyMesh mMesh;

		vcg::GlTrimesh<MyMesh> glTriMesh; // wraps the mesh and draws it
	};
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Mesh&);

#endif
