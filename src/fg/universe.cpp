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

#include "fg/universe.h"
#include "fg/bindings.h"

#include <lauxlib.h>
#include <lualib.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <cmath>
#include <cstring>
#include <ctime>

#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/function.hpp>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>

#include "fg/node.h"
#include "fg/meshnode.h"
#include "fg/nodegraph.h"

namespace fg {

	/*
	void error(const char* msg){
		std::cerr << msg;
		std::exit(-1);
	}
	*/

	/*
	void error(const char* msg, const char* args){
		std::fprintf(stderr, msg, args);
		std::exit(-1);
	}
	*/

	void error(const char* msg, ... ){
		va_list arg_list;
		va_start(arg_list, msg);
		std::vfprintf(stderr, msg, arg_list);
		va_end(arg_list);
		std::exit(-1);
	}

	Universe::Universe():
	L(NULL),
	mLoadedScripts(),
	mMeshes(),
	mTime(0),
	mNodeGraph()
	{
		// Global setup
		std::srand(std::time(NULL));

		// Lua setup
		L = lua_open();   /* opens Lua */
		luaL_openlibs(L); /* opens the base libraries */
		// init(L);
		fg::loadLuaBindings(L);

		// setup a debugging error function
		lua_register(L, "fgerrorfunc", debugFileAndLine);
	}

	Universe::~Universe() {
		lua_close(L);
	}

	void Universe::addScriptDirectory(std::string scriptPath){
		/*
		lua_getglobal(L,"package.path");
		std::string path;
		if (lua_isstring(L,-1))
			path = lua_tostring(L,-1);
		lua_pop(L,1);
		if (!path.empty()) path += ";";
		path += scriptPath;
		std::cout << "Modifying package.path: \"" << path << "\"\n";
		lua_pushstring(L,path.c_str());
		lua_setglobal(L,"package.path");
		*/
		setLuaPath(scriptPath);
	}

	void Universe::loadScript(std::string scriptFileName){
		// Load the script, and give it access to this universe
		// which is called "fgu"


		// // Adapted from lua.c:  static int dolibrary (lua_State *L, const char *name) {
		lua_getglobal(L, "require");
		lua_pushstring(L, scriptFileName.c_str());
		if (lua_pcall(L, 1, 1, 0)){
			error(lua_tostring(L, -1));
		}
		else {
			// check that the script is a module of the correct form
			if (!lua_istable(L,-1)){
				error("Script %s is not a module",scriptFileName.c_str());
			}
			else {
				// add the encompassing universe fgu object
				// first check to see that it doesnt exist
				lua_getfield(L,-1,"fgu");
				if(!lua_isnil(L,-1)){
					error("Script \"%s\" overrides the \"fgu\" universe!", scriptFileName.c_str());
				}
				lua_pop(L,1); // pop fgu ref

				// then add it
				// get the module table
				luabind::object table(luabind::from_stack(L,-1));
				if (luabind::type(table) == LUA_TTABLE){
					table["fgu"] = this;
				}
				else {
					error("Script \"%s\" doesn't look like a table to luabind", scriptFileName.c_str());
				}

				// handle setup with fg's error handler
				lua_getglobal(L, "fgerrorfunc");

				// check that the module has a setup function
				lua_getfield(L,-2,"setup");

				if (!lua_isfunction(L,-1)){
					error("Script \"%s\" doesn't have a setup function", scriptFileName.c_str());
				}
				else // call the setup function now..
				{
					if (lua_pcall(L, 0, 0, -2)!=0){ // NB: The "-2" references fgerrorfunc
						error("! Error in %s.setup()\n! %s", scriptFileName.c_str(), lua_tostring(L, -1));
						// debugFileAndLine(L);
					}
				}

				// check that the module has an update function
				lua_getfield(L,-2,"update");
				if (!lua_isfunction(L,-1)){
					error("Script \"%s\" doesn't have a update function",scriptFileName.c_str());
				}
				else // keep a reference to this function so we can call it later
				{
					lua_setglobal(L,(scriptFileName + "_update").c_str());
					mLoadedScripts.push_back(scriptFileName);
				}
			}
			lua_pop(L,1); // pop table
		}
	}

	void Universe::addMesh(boost::shared_ptr<Mesh> m){
		add(boost::shared_ptr<MeshNode>(new MeshNode(m)));
	}

	Universe::MeshContainer& Universe::meshes(){
		throw("Universe::meshes() is deprecated!");
	}

	void Universe::add(boost::shared_ptr<Node> n){
		mNodeGraph.addNode(n);
		mNodes.push_back(n);
	}

	void Universe::add(boost::shared_ptr<MeshNode> n){
		mNodeGraph.addNode(n);
		mNodes.push_back(n);
		mMeshNodes.push_back(n);
	}

	std::list<boost::shared_ptr<MeshNode> >& Universe::meshNodes(){
		return mMeshNodes;
	}

	std::list<boost::shared_ptr<Node> >& Universe::nodes(){
		return mNodes;
	}

	void Universe::makeChildOf(boost::shared_ptr<Node> parent, boost::shared_ptr<Node> child){
		mNodeGraph.addEdge(parent,child);
	}

	/**
	 * Update all objects in the universe by a specified time increment.
	 */
	void Universe::update(double dt){
		BOOST_FOREACH(std::string s, mLoadedScripts)
		{
			lua_getglobal(L,(s + "_update").c_str());
			if (lua_isfunction(L,-1)){
				lua_pushnumber(L, dt);
				if (lua_pcall(L, 1, 0, 0)!=0) {
					// debugFileAndLine(L);
					error("Error running function: %s",lua_tostring(L, -1));
				}
			}
			else {
				error("Script \"%s\" doesn't have a update function",s.c_str());
			}
		}

		mNodeGraph.update();

		mTime += dt;
	}

	double Universe::time() const ///< Returns the current time in the universe
	{
		return mTime;
	}

	// from http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c
	int Universe::setLuaPath( std::string path )
	{
		std::cout << "Adding script path: \"" << path << "\"\n";

	    lua_getglobal( L, "package" );
	    lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
	    std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack
	    cur_path.append( ";" ); // do your path magic here
	    cur_path.append( path );
	    lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
	    lua_pushstring( L, cur_path.c_str() ); // push the new one
	    lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
	    lua_pop( L, 1 ); // get rid of package table from top of stack
	    return 0; // all done!
	}

	int Universe::debugFileAndLine(lua_State* L)
	{
	   lua_Debug d;
	   lua_getstack(L, 1, &d);
	   lua_getinfo(L, "Sln", &d);
	   std::string err = lua_tostring(L, -1);
	   lua_pop(L, 1);
	   std::stringstream msg;
	   msg << d.short_src << ":" << d.currentline;

	   if (d.name != 0)
	   {
	      msg << "(" << d.namewhat << " " << d.name << ")";
	   }
	   msg << " " << err;
	   lua_pushstring(L, msg.str().c_str());
	   return 1;
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Universe& u){
	return o << "universe " << &u;
}

