#include "fg/bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>

#include <sstream>

#include "fg/universe.h"
#include "fg/functions.h"
#include "fg/mesh.h"
#include "fg/vec3.h"

int debugFileAndLine(lua_State* L);

namespace fg {
	int loadLuaBindings(lua_State* L){
		using namespace luabind;

		//set_pcall_callback(debugFileAndLine);
		open(L);

		// fg/universe.h
		module(L,"fg")[
		   class_<fg::Universe>("universe")
		   .def(tostring(const_self))
		   .def("addMesh", &fg::Universe::addMesh)
		   .property("t", &fg::Universe::time)
		   .def("time", &fg::Universe::time)
		];

		// fg/functions.h
		module(L,"fg")[
		   def("min", &min<double>),
		   def("lerp", &lerp<double>)
		   ];

		// fg/vec3.h
		module(L, "fg")[
		   class_<fg::Vec3>("vec3")
		   .property("x", &fg::Vec3::getX, &fg::Vec3::setX)
		   .property("y", &fg::Vec3::getY, &fg::Vec3::setY)
		   .property("z", &fg::Vec3::getZ, &fg::Vec3::setZ)
		   .def(tostring(const_self))
		];

		// fg/mesh.h
		module(L,"fg")[
		   class_<fg::Mesh::Vertex>("vertex")
		   .def("getPosition", &fg::Mesh::Vertex::pos)
		   .property("pos", &fg::Mesh::Vertex::pos)
		   .def(tostring(const_self)),

		   // class_<fg::Mesh::VertexContainer>("vertexcontainer"),

		   class_<fg::Mesh::VertexSet>("vertexset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::VertexSet>, return_stl_iterator),

		   class_<fg::Mesh>("mesh")
		   // .def("vertices",&Mesh::vertices, return_stl_iterator)
		   .def("selectAllVertices", &Mesh::selectAllVertices)
		   .def(tostring(const_self))
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
