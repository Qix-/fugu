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

	// A facade to the vertex implementation
	class Vertex {
	public:
		virtual Vec3& pos() = 0;
	};

	class Face {
	public:

	};

	class MeshImpl;
	class Mesh {
		public:
		typedef std::list<Vertex*> VertexSet;

		// Mesh factories
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
		};
		~Mesh();

		// Accessors
		// VertexContainer& vertices();
		// VertexSet vertices();
		boost::shared_ptr<VertexSet> selectAllVertices();

		// Queries
		void getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz);

		// Common modifiers
		void subdivide(int levels); ///< Perform smooth subdivision on the mesh

		// OpenGL helpers
		void drawGL();

		private:
		Mesh(); // Can't construct a blank mesh.
		MeshImpl* mpMesh;
	};
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Mesh&);
std::ostream& operator<<(std::ostream&, const fg::Vertex&);


#endif
