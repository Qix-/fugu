#ifndef FG_MESH_H
#define FG_MESH_H

/** \brief A triangular mesh.
 *
 * Implementation notes:
 * - It wraps a vcglib mesh with as much info as we can jam in...
 *
 */

// fg dependencies
#include "fg/vec3.h"
#include "fg/vertex.h"

// Stdlibs
#include <ostream>
#include <vector>
#include <list>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

// for opengl
#include "GL/glew.h"
#include <wrap/gl/trimesh.h>

namespace fg {
	class Face {
	public:

	};

	class MeshImpl;
	class Mesh {
		public:
		typedef std::list<boost::shared_ptr<VertexProxy> > VertexSet;

		/**
		 * Pre-defined mesh primitives
		 */
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
			static boost::shared_ptr<Mesh> Sphere();
		};

		/**
		 * \brief Construct a mesh manually from vertices and triangles
		 *
		 * A MeshBuilder can be used as a factory object, e.g.,
		 *
		 * MeshBuilder mb;
		 * mb.addVertex(...)
		 * mb.addFace(...)
		 * mb.createMesh()
		 *
		 * ...or with the static function MeshBuilder::createMesh(vert_array,face_array)
		 */
		class MeshBuilder {
		public:
			MeshBuilder();
			~MeshBuilder();
			void addVertex(double x, double y, double z);
			void addFace(int v1, int v2, int v3);
			boost::shared_ptr<Mesh> createMesh();

			static boost::shared_ptr<Mesh> createMesh(const std::vector<Vec3>& verts, const std::vector<boost::tuple<int,int,int> >& faces);
		protected:
			std::vector<Vec3> mVertices;
			std::vector<boost::tuple<int,int,int> > mTriangles;
		};

		~Mesh();

		// Accessors
		/**
		 * Select all the vertices
		 * TODO: Update to new VertexProxy system
		 * @return
		 */
		boost::shared_ptr<VertexSet> selectAllVertices();

		/**
		 * @return A random vertex
		 */
		boost::shared_ptr<VertexProxy> selectRandomVertex();

		// Queries
		void getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz);

		// Common modifiers
		void subdivide(int levels); ///< Perform flat subdivision on the mesh
		void smoothSubdivide(int levels); ///< Perform smooth subdivision on the mesh

		/// Sync will make sure all the topology, normals, etc are fixed..
		void sync();

		/// @deprecated see GLRenderer
		void drawGL();



		/**
		 * Low-level implementation access.
		 * Only use these if you know what you are doing.
		 */

		/// return the VCG implementation in this mesh
		MeshImpl* _impl();

		/// return the VertexProxyList, so vertices can get their references updated
		VertexProxyList* _vpl();

		private:
		Mesh(); // Can't construct a blank mesh.
		MeshImpl* mpMesh;
		VertexProxyList mVertexProxyList;
	};
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Mesh&);


#endif
