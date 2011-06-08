#ifndef FG_MESH_H
#define FG_MESH_H

/**
 * fg::Mesh is a triangular mesh.
 *
 * Implementation notes:
 * - It wraps a VCG mesh with as much info as we can jam in...
 *
 */

// fg dependencies
#include "fg/vec3.h"

// Stdlibs
#include <ostream>
#include <vector>
#include <list>

// VCG
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

// BOOST
#include <boost/shared_ptr.hpp>

// for opengl
#include "GL/glew.h"
#include <wrap/gl/trimesh.h>

namespace fg {
	class Mesh {
		/** VCG Implementation **/
		public:
		class MyVertex;
		class MyFace;

		struct MyUsedTypes : public vcg::UsedTypes<
			vcg::Use<MyVertex>::AsVertexType,
			vcg::Use<MyFace>::AsFaceType>{};

		public:
		class MyVertex: public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3d, vcg::vertex::Normal3d, vcg::vertex::BitFlags, vcg::vertex::VFAdj, vcg::vertex::InfoOcf, vcg::vertex::Mark>{
			// Adapt the interface
			public:
			Vec3& pos(){return static_cast<Vec3&>(P());}
		};
		class MyFace: public vcg::Face< MyUsedTypes, vcg::face::FFAdj, vcg::face::Mark, vcg::face::VertexRef, vcg::face::Normal3d, vcg::face::BitFlags, vcg::face::InfoOcf> {};
		class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};
		/************************/

		public:
		typedef MyVertex Vertex;
		typedef MyMesh::VertContainer VertexContainer;
		typedef std::list<Vertex*> VertexSet;

		// Mesh factories
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
		};
		~Mesh();

		// Accessors
		VertexContainer& vertices();
		// VertexSet vertices();
		boost::shared_ptr<VertexSet> selectAllVertices();

		// Queries
		void getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz);

		// Common modifiers
		void subdivide(int levels); ///< Perform smooth subdivision on the mesh

		// OpenGL helpers
		void drawGL();

		private:
		Mesh(); // can't create a mesh from nothing .. yet
		MyMesh mMesh;

		vcg::GlTrimesh<MyMesh> glTriMesh; // wraps the mesh and draws it
	};
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Mesh&);
std::ostream& operator<<(std::ostream&, const fg::Mesh::Vertex&);


#endif
