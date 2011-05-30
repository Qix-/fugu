#include "fg/universe.h"

#include <lauxlib.h>
#include <lualib.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>

//#define BOOST_FILESYSTEM_VERSION 3
//#include <boost/filesystem.hpp>

namespace fg {
	void error(const char* msg){
		std::cerr << msg;
		std::exit(-1);
	}

	void error(const char* msg, const char* args){
		std::fprintf(stderr, msg, args);
		std::exit(-1);
	}

	Universe::Universe() {
		L = lua_open();   /* opens Lua */
		luaL_openlibs(L); /* opens the base libraries */
		// init(L);
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
				lua_pushstring(L,"setup");
				lua_gettable(L,-2);
				if (!lua_isfunction(L,-1)){
					error("Script %s doesn't have a setup function",scriptFileName.c_str());
				}
				else // call the setup function now..
				{
					if (lua_pcall(L, 0, 0, 0)!=0){
						error("Error running function \"g\": %s",lua_tostring(L, -1));
					}
				}
			}
			lua_pop(L,1); // pop table
		}
	}

	/**
	 * Update all objects in the universe by a specified time increment.
	 */
	void Universe::update(double dt){
		; // do nothing for now
	}

	// from http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c
	int Universe::setLuaPath( std::string path )
	{
	    lua_getglobal( L, "package" );
	    lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
	    std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack
	    cur_path.append( ";" ); // do your path magic here
	    cur_path.append( path );
	    std::cout << "Modifying script path: \"" << cur_path << "\"\n";
	    lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
	    lua_pushstring( L, cur_path.c_str() ); // push the new one
	    lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
	    lua_pop( L, 1 ); // get rid of package table from top of stack
	    return 0; // all done!
	}
}
