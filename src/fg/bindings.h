/**
 * \file
 * \brief Generates lua bindings for fg
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

#ifndef FG_BINDINGS_H
#define FG_BINDINGS_H

#include <lua.hpp>
namespace fg {
	/**
	 * \brief Loads the lua bindings for the fg library into the current lua state.
	 */
	int loadLuaBindings(lua_State* L);

	// Helpers
	template <class STLContainer>
	STLContainer& luaAllAdapter(STLContainer* s){
		return *s;
	}
}

#endif
