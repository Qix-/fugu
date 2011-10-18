/**
 * \file
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

#ifndef FG_UNIVERSE_H
#define FG_UNIVERSE_H

#include <fstream>
#include <string>
#include <list>

#include <lua.hpp>
#include <boost/shared_ptr.hpp>

#include "fg/node.h"
#include "fg/meshnode.h"
#include "fg/nodegraph.h"

namespace fg {

	// Forward Decl
	class Mesh;

	/**
	 * \brief Contains objects, meshes, scripts, etc. for an fg simulation.
	 *
	 * A fg::Universe provides the main entrypoint to an fg simulation. Typically, you will use
	 * exactly one universe in your fg simulation. The universe has a C++ API as defined here, but also provides
	 * functions to the lua scripts loaded within it.
	 */
	class Universe {
	public:
		typedef std::list<boost::shared_ptr<Mesh> > MeshContainer;

		Universe();
		~Universe();

		void addScriptDirectory(std::string scriptPath);

		/** \brief Load an fg script.
		 *
		 * This method loads a lua script and executes its setup function.
		 * It will also add it to the list of scripts to be updated if it
		 * has an update function.
		 */
		void loadScript(std::string fileName);

		/** \brief Immediately run the lua code passed in script.
		 *
		 * @param script A string containing the lua code to execute.
		 */
		void runScript(std::string script);

		/**
		 * /brief Add a mesh to the universe
		 *
		 * @deprecated Use add(node) instead. (This function now adds a mesh node internally)
		 */
		void addMesh(boost::shared_ptr<Mesh> m);

		/// @deprecated Don't use this!
		// void deleteMesh(boost::shared_ptr<Mesh> m);

		/**
		 * /brief Retrieve all the meshes in this universe
		 *
		 * @deprecated Use meshNodes() instead. This function will now throw an exception.
		 */
		MeshContainer& meshes();

		/// Add a node to this universe
		void add(boost::shared_ptr<Node> n);

		/// Add a mesh node to this universe
		void add(boost::shared_ptr<MeshNode> n);

		/// Retrieve all the nodes
		std::list<boost::shared_ptr<Node> >& nodes();

		/// Retrieve all the mesh nodes
		std::list<boost::shared_ptr<MeshNode> >& meshNodes();


		/// Bind a child node to the coordinate system of a parent node
		void makeChildOf(boost::shared_ptr<Node> parent, boost::shared_ptr<Node> child);

		/**
		 * /brief Update all modules and nodes in the universe by a specified time increment.
		 *
		 * NOTE: nodes are updated after the module updates
		 */
		void update(double dt);

		// helpers
		double time() const; ///< Returns the current time in the universe

		/// \brief Return a list of all categorised commands in tuple(category,name,docstring)
		std::list<tuple<std::string,std::string,std::string> > commandListByCategory() const;
	private:
		lua_State *L;
		std::list<std::string> mLoadedScripts;
		MeshContainer mMeshes;

		double mTime; ///< universe time
		NodeGraph mNodeGraph;
		bool mChangedNodeGraph;

		std::list<boost::shared_ptr<Node> > mNodes;
		std::list<boost::shared_ptr<MeshNode> > mMeshNodes;

	private: // helpers
		int setLuaPath( std::string path );
		static int debugFileAndLine(lua_State* L);
	};
}

std::ostream& operator<<(std::ostream&, const fg::Universe&);

#endif
