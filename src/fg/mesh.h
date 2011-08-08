/**
 * \file
 * \brief Declares fg::Mesh
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
 
#ifndef FG_MESH_H
#define FG_MESH_H

// fg dependencies
#include "fg/vec3.h"
#include "fg/vertex.h"
#include "fg/mat4.h"
#include "fg/util.h"

// Stdlibs
#include <ostream>
#include <vector>
#include <list>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

// for opengl
#include "GL/glew.h"
#include <wrap/gl/trimesh.h>

namespace fg {
	/**
	 * \brief A triangular face in an fg::Mesh
	 */
	class Face {
	public:
		// Currently empty
	};

	// forward decl
	class MeshImpl;
	
	/** 
	 * \brief A triangular mesh in an fg simulation
	 *
	 * NOTE: It wraps a vcglib mesh (see fg::MeshImpl)
	 *
	 */
	class Mesh {
		public:
		typedef std::list<boost::shared_ptr<VertexProxy> > VertexSet;

		/**
		 * \brief Common mesh primitives.
		 *
		 * (Adapters to vcg primitives)
		 * TODO: Iso-surface?
		 */
		class Primitives {
			public:

			static boost::shared_ptr<Mesh> Cube();
			static boost::shared_ptr<Mesh> Icosahedron();
			static boost::shared_ptr<Mesh> Sphere();
			static boost::shared_ptr<Mesh> Tetrahedron();
			static boost::shared_ptr<Mesh> Dodecahedron();
			static boost::shared_ptr<Mesh> Octahedron();
			// static boost::shared_ptr<Mesh> Hexahedron();
			static boost::shared_ptr<Mesh> Cone(double r1, double r2, const int SubDiv = 36);
			static boost::shared_ptr<Mesh> Cylinder(int slices); // , int stacks);

			private:
			static boost::shared_ptr<Mesh> sync(Mesh* m);

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
			void addVertex(Vec3 pos);

			void addVertexColour(Vec3 colour);
			void addVertexUV(double u, double v);

			void addFace(int v1, int v2, int v3);
			boost::shared_ptr<Mesh> createMesh();
			int getNumVertices();

			int getNumVerticies(); ///< \deprecated spelling error

			static boost::shared_ptr<Mesh> createMesh(const std::vector<Vec3>& verts, const std::vector<boost::tuple<int,int,int> >& faces);
		protected:
			std::vector<Vec3> mVertices;
			std::vector<Vec3> mVertexColours;
			std::vector<pair<double,double> > mVertexUVs;
			std::vector<boost::tuple<int,int,int> > mTriangles;
		};

		static boost::shared_ptr<Mesh> Load(std::string path);
		static boost::shared_ptr<Mesh> Load(boost::filesystem::path file);

		~Mesh();

		// Accessors
		
		/**
		 * \brief Select (retrieve) all the vertices in this mesh
		 */
		boost::shared_ptr<VertexSet> selectAllVertices();

		/**
		 *\brief Select a random vertex
		 */
		boost::shared_ptr<VertexProxy> selectRandomVertex();

		// Queries
		void getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz);

		// Common modifiers
		void subdivide(int levels); ///< \brief Perform flat subdivision on the mesh
		void smoothSubdivide(int levels); ///< \brief Perform smooth subdivision on the mesh

		/// \brief Sync will make sure all the topology, normals, etc are fixed..
		void sync();

		void applyTransform(const Mat4& T); ///< \brief Applies T to the positions of the vertices. (For each vertex v in mesh, v.pos = T*v.pos) Also syncs at the end so the normals are appropriate.

		/**
		 * \brief TODO: Merges mesh m into this mesh. NOTE: m is now invalid.
		 */
		// void merge(boost::shared_ptr<Mesh> m);

		/// \deprecated see fg::GLRenderer
		void drawGL();

		/*
		 * Low-level implementation access.
		 * Only use these if you know what you are doing.
		 */

		/// \brief (LOW LEVEL) Return the VCG implementation in this mesh
		MeshImpl* _impl();

		/// \brief (LOW LEVEL) return the VertexProxyList, so vertices can get their references updated
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
