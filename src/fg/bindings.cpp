#include "fg/bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>

#include "fg/functions.h"

namespace fg {
	int loadLuaBindings(lua_State* L){
		using namespace luabind;
		open(L);

		// fg/functions.h
		module(L,"fg")[
		          def("min", &min<double>),
		          def("lerp", &lerp<double>)
		          ];

		return 0;
	}
}
