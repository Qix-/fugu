#ifndef FG_UNIVERSE_H
#define FG_UNIVERSE_H

#include <fstream>
#include <string>
#include <lua.hpp>

namespace fg {

	/**
	 * A universe contains all the objects, meshes, scripts, etc.
	 */
	class Universe {
	public:
		Universe();
		~Universe();

		void addScriptDirectory(std::string scriptPath);

		void loadScript(std::string scriptName);

		/**
		 * Update all objects in the universe by a specified time increment.
		 */
		void update(double dt);

	private:
		lua_State *L;

	private: // helpers
		int setLuaPath( std::string path );
	};
}

#endif
