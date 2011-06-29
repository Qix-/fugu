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

		// Mesh factories
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
			static boost::shared_ptr<Mesh> Sphere();
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
		void subdivide(int levels); ///< Perform smooth subdivision on the mesh

		/// Sync will make sure all the topology, normals, etc are fixed..
		void sync();

		// OpenGL helpers
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
