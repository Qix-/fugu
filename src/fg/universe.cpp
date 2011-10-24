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
#include <exception>

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

	/*
	void error(const char* msg, ... ){
		va_list arg_list;
		va_start(arg_list, msg);
		std::vfprintf(stderr, msg, arg_list);
		va_end(arg_list);
		std::exit(-1);
	}
	*/

	void error(const char* msg, ... ){
		va_list arg_list;
		va_start(arg_list, msg);
		char expandedMessage[1024];
		std::vsprintf(expandedMessage, msg, arg_list);
		va_end(arg_list);

		throw(std::runtime_error(std::string(expandedMessage)));
	}

	Universe::Universe():
	L(NULL),
	mLoadedScripts(),
	mMeshes(),
	mTime(0),
	mNodeGraph(),
	mChangedNodeGraph(false)
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

		// and finally load the core library..
		setLuaPath("../?.lua");  // assume this is run from inside the bin/
		lua_getglobal(L, "require");
		lua_pushstring(L, "core.main");
		if (lua_pcall(L, 1, 1, 0)){
			error(lua_tostring(L, -1));
		}
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
						error("Lua error in %s.setup()\n! %s", scriptFileName.c_str(), lua_tostring(L, -1));
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

	/** \brief Immediately run the lua code passed in script.
	 *
	 * @param script A string containing the lua code to execute.
	 */
	void Universe::runScript(std::string script){
		// Make sure the code can access this universe
		luabind::globals(L)["fgu"] = this;

		const char* buff = script.c_str();
		luaL_loadbuffer(L, buff, script.length(), "line");
		if (lua_pcall(L, 0, 0, 0)){
			// an error has occurred
			try {
				error(lua_tostring(L, -1));
			}
			catch (std::runtime_error& e){
				std::cerr << e.what() << "\n";
			}
			lua_pop(L,1);
		}
	}

	void Universe::addMesh(boost::shared_ptr<Mesh> m){
		add(boost::shared_ptr<MeshNode>(new MeshNode(m)));

		mChangedNodeGraph = true;
	}

	Universe::MeshContainer& Universe::meshes(){
		throw("Universe::meshes() is deprecated!");
	}

	void Universe::add(boost::shared_ptr<Node> n){
		mNodeGraph.addNode(n);
		mNodes.push_back(n);

		mChangedNodeGraph = true;
	}

	void Universe::add(boost::shared_ptr<MeshNode> n){
		mNodeGraph.addNode(n);
		mNodes.push_back(n);
		mMeshNodes.push_back(n);

		mChangedNodeGraph = true;
	}

	std::list<boost::shared_ptr<MeshNode> >& Universe::meshNodes(){
		return mMeshNodes;
	}

	std::list<boost::shared_ptr<Node> >& Universe::nodes(){
		return mNodes;
	}

	void Universe::makeChildOf(boost::shared_ptr<Node> parent, boost::shared_ptr<Node> child){
		mNodeGraph.addEdge(parent,child);

		mChangedNodeGraph = true;
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
					error("Lua error: %s",lua_tostring(L, -1));
				}
			}
			else {
				error("Script \"%s\" doesn't have a update function",s.c_str());
			}
		}

		if (mChangedNodeGraph){
			mNodeGraph.recomputeOrdering();
			mChangedNodeGraph = false;
		}

		mNodeGraph.update();

		mTime += dt;
	}

	double Universe::time() const ///< Returns the current time in the universe
	{
		return mTime;
	}

	std::list<tuple<std::string,std::string,std::string> > Universe::commandListByCategory() const {
		std::list<tuple<std::string,std::string,std::string> > l;
		// std::cout << "doing a thing..\n";
		// run command_list_by_category()
		// and parse the result
		// TODO: this..
		const char* command = "return command_list_by_category()";
		if (luaL_dostring(L, command)){
			// an error has occurred
			try {
				error(lua_tostring(L, -1));
			}
			catch (std::runtime_error& e){
				std::cerr << e.what() << "\n";
			}
			lua_pop(L,1);
		}
		else {
			// extract result
			luabind::object o(luabind::from_stack(L, -1));
			if (o.is_valid() and luabind::type(o)==LUA_TTABLE){
				// std::cerr << "looks like a table...\n";
				for (luabind::iterator i(o), end; i != end; ++i)
				{
					// std::cerr << "can iterate like a table...\n";
					if (luabind::type(*i)==LUA_TTABLE){
						luabind::object cat = (*i)["category"];
						luabind::object name = (*i)["name"];
						luabind::object docstring = (*i)["docstring"];

						if (not cat.is_valid() or not luabind::type(cat)==LUA_TSTRING) continue;

						l.push_back(make_tuple(
								luabind::object_cast<std::string>(cat),
								(name.is_valid() and luabind::type(name)==LUA_TSTRING)?luabind::object_cast<std::string>(name):std::string(),
								(docstring.is_valid() and luabind::type(docstring)==LUA_TSTRING)?luabind::object_cast<std::string>(docstring):std::string()
						));
					}
				}
			}
			else {
				std::cerr << "Can't execute command_list_by_category() for some reason...";
			}
			lua_pop(L,1);
		}
		return l;
	}

	// from http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c
	int Universe::setLuaPath( std::string path )
	{
		// std::cout << "Adding script path: \"" << path << "\"\n";

	    lua_getglobal( L, "package" );
	    lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
	    std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack

	    std::string new_path(path);
	    new_path.append(";");
	    new_path.append(cur_path);
	    // cur_path.append( ";" ); // do your path magic here
	    // cur_path.append( path );
	    lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
	    lua_pushstring( L, new_path.c_str() ); // push the new one
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

