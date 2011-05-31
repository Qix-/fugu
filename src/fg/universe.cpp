#include "fg/universe.h"
#include "fg/bindings.h"

#include <lauxlib.h>
#include <lualib.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>

//#define BOOST_FILESYSTEM_VERSION 3
//#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fg {
	void error(const char* msg){
		std::cerr << msg;
		std::exit(-1);
	}

	void error(const char* msg, const char* args){
		std::fprintf(stderr, msg, args);
		std::exit(-1);
	}

	Universe::Universe():
	L(NULL),
	loadedScripts()
	{
		L = lua_open();   /* opens Lua */
		luaL_openlibs(L); /* opens the base libraries */
		// init(L);
		fg::loadLuaBindings(L);

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
				// check that the module has a setup function
				lua_pushstring(L,"setup");
				lua_gettable(L,-2);
				if (!lua_isfunction(L,-1)){
					error("Script \"%s\" doesn't have a setup function",scriptFileName.c_str());
				}
				else // call the setup function now..
				{
					if (lua_pcall(L, 0, 0, 0)!=0){
						error("Error running function: %s",lua_tostring(L, -1));
					}
				}

				// check that the module has an update function
				lua_pushstring(L,"update");
				lua_gettable(L,-2);
				if (!lua_isfunction(L,-1)){
					error("Script \"%s\" doesn't have a update function",scriptFileName.c_str());
				}
				else // keep a reference to this function so we can call it later
				{
					lua_setglobal(L,(scriptFileName + "_update").c_str());
					loadedScripts.push_back(scriptFileName);
				}
			}
			lua_pop(L,1); // pop table
		}
	}

	/**
	 * Update all objects in the universe by a specified time increment.
	 */
	void Universe::update(double dt){
		BOOST_FOREACH(std::string s, loadedScripts)
		{
			lua_getglobal(L,(s + "_update").c_str());
			if (lua_isfunction(L,-1)){
				lua_pushnumber(L, dt);
				if (lua_pcall(L, 1, 0, 0)!=0) {
					error("Error running function: %s",lua_tostring(L, -1));
				}
			}
			else {
				error("Script \"%s\" doesn't have a update function",s.c_str());
			}
		}
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
}
