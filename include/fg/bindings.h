#ifndef FG_BINDINGS_H
#define FG_BINDINGS_H

/**
 * fg::loadLuaBindings loads the lua bindings for the fg library into
 * the current lua state.
 *
 * All bindings should go in bindings.cpp.
 */

#include <lua.hpp>
namespace fg {
	int loadLuaBindings(lua_State* L);

	// Helpers
	template <class STLContainer>
	STLContainer& luaAllAdapter(STLContainer* s){
		return *s;
	}
}

#endif
