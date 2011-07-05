#include "fg/bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>

#include <sstream>

#include "fg/vec3.h"
#include "fg/universe.h"
#include "fg/functions.h"
#include "fg/mesh.h"
#include "fg/meshoperators.h"


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
		   def("lerp", &lerp<double>),

		   def("noise", (double(*)(double)) &noise),
		   def("noise", (double(*)(double,double)) &noise),
		   def("noise", (double(*)(double,double,double)) &noise),

		   def("random", (double(*)()) &fg::random),
		   def("random", (double(*)(double,double)) &fg::random)
		   ];

		// fg/vec3.h
		module(L, "fg")[
		   class_<fg::Vec3>("vec3")
		   .def(constructor<double,double,double>())
		   .property("x", &fg::Vec3::getX, &fg::Vec3::setX)
		   .property("y", &fg::Vec3::getY, &fg::Vec3::setY)
		   .property("z", &fg::Vec3::getZ, &fg::Vec3::setZ)
		   .def(tostring(const_self))

		   .def("normalise",&fg::Vec3::normalise)
		   .def(const_self + other<fg::Vec3>())
		   .def(const_self - other<fg::Vec3>())
		   .def(const_self * double())
		];

		// fg/mesh.h
		module(L,"fg")[
		   class_<fg::VertexProxy>("vertex")
		   .def(tostring(const_self))

		   // Properties
		   .def("getPos", &fg::VertexProxy::getPos)
		   .def("setPos", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))
		   .def("setPos", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setPos))
		   // .property("pos", &fg::VertexProxy::getPos, &fg::VertexProxy::setPos)

		   .def("getN", &fg::VertexProxy::getN)
		   // class_<fg::Mesh::VertexContainer>("vertexcontainer"),

		   .property("valid", &fg::VertexProxy::isValid),

		   class_<fg::Mesh::VertexSet>("vertexset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::VertexSet>, return_stl_iterator),

		   class_<fg::Mesh>("mesh")
		   .def(tostring(const_self))

		   // Selectors
		   .def("selectAllVertices", &Mesh::selectAllVertices)
		   .def("selectRandomVertex", &Mesh::selectRandomVertex)

		   .def("subdivide", &Mesh::subdivide)
		   .def("smoothSubdivide", &Mesh::smoothSubdivide)
		   .def("sync", &Mesh::sync)

		   .scope [
			   class_<fg::Mesh::Primitives>("primitives")
			   .scope [
					   def("icosahedron",&fg::Mesh::Primitives::Icosahedron),
					   def("sphere",&fg::Mesh::Primitives::Sphere)
			   ]
		   ]

		   .scope [
		        def("load", (boost::shared_ptr<Mesh>(*)(std::string)) &Mesh::Load)
		   ]
		];

		// fg/meshoperators.h
		module(L,"fg")[
		   // def("extrude", &fg::extrude),
		   def("extrude", (void(*)(Mesh*,VertexProxy,double))&fg::extrude),
		   def("extrude", (void(*)(Mesh*,VertexProxy,int,Vec3,double,double))&fg::extrude),

		   def("getVerticesAtDistance", getVerticesAtDistance)
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
