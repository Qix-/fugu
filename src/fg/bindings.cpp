#include "fg/bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>

#include <sstream>

#include "fg/functions.h"
#include "fg/mesh.h"

int debugFileAndLine(lua_State* L);

namespace fg {
	int loadLuaBindings(lua_State* L){
		using namespace luabind;

		//set_pcall_callback(debugFileAndLine);
		open(L);

		// fg/functions.h
		module(L,"fg")[
		   def("min", &min<double>),
		   def("lerp", &lerp<double>)
		   ];

		module(L,"fg")[
		   class_<fg::Mesh>("mesh")
		   .def(luabind::tostring(luabind::const_self)) // or const_self?
		   .scope [
			   class_<fg::Mesh::Primitives>("primitives")
			   .scope [
					   def("icosahedron",&fg::Mesh::Primitives::Icosahedron)
			   ]
		   ]
		];

		return 0;
	}
}

int debugFileAndLine(lua_State* L)
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
