#ifndef FG_UNIVERSE_H
#define FG_UNIVERSE_H

#include <fstream>
#include <string>
#include <list>

#include <lua.hpp>
#include <boost/shared_ptr.hpp>

namespace fg {

	// Forward Decl
	class Mesh;

	/**
	 * A universe contains all the objects, meshes, scripts, etc.
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
		void loadScript(std::string scriptName);

		void addMesh(boost::shared_ptr<Mesh> m);
		MeshContainer& meshes();

		/** Update all objects in the universe by a specified time increment.
		 */
		void update(double dt);

	private:
		lua_State *L;
		std::list<std::string> mLoadedScripts;
		MeshContainer mMeshes;

	private: // helpers
		int setLuaPath( std::string path );
		static int debugFileAndLine(lua_State* L);
	};
}

std::ostream& operator<<(std::ostream&, const fg::Universe&);

#endif
