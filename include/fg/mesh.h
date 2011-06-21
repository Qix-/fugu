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

	/*
	 * All the proxies follow the following general pattern.
	 */
	/*
	template <class T>
	class Proxy {
	public:
		Proxy(T* t):mImpl(t){}
		T* impl(){return mImpl;}
	protected:
		T* mImpl;
	};
	*/

	// A proxy to the vertex implementation
	class VertexImpl;
	class VertexProxy {
	public:
		VertexProxy(VertexImpl* vi);
		~VertexProxy();

		//Vec3& pos();
		//Vec3& normal();
		Vec3 getPos() const;
		void setPos(Vec3 v);
		void setPos(double x, double y, double z);

		VertexImpl* impl();
	private:
		VertexImpl* mImpl;
	};

	class Face {
	public:

	};

	class MeshImpl;
	class Mesh {
		public:
		typedef std::list<VertexProxy> VertexSet;

		// Mesh factories
		class Primitives {
			public:
			static boost::shared_ptr<Mesh> Icosahedron();
			static boost::shared_ptr<Mesh> Sphere();
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

		// return the VCG implementation in this mesh
		MeshImpl* impl();

		private:
		Mesh(); // Can't construct a blank mesh.
		MeshImpl* mpMesh;
	};
}

// For basic printing
std::ostream& operator<<(std::ostream&, const fg::Mesh&);
std::ostream& operator<<(std::ostream&, const fg::VertexProxy&);


#endif
