/**
 * \file
 * \brief Defines the lua bindings for fg
 * \author ben jonmc wetterj
 * 
 * All lua bindings for fg are in this file. 
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

#include "fg/bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>

#include <sstream>

#include "fg/vec3.h"
#include "fg/mat4.h"
#include "fg/universe.h"
#include "fg/functions.h"

#include "fg/node.h"

#include "fg/mesh.h"
#include "fg/meshnode.h"
#include "fg/meshoperators.h"

#include "fg/turtle.h"

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
		   .def("addMesh", &fg::Universe::addMesh) // deprecated..
		   .def("add", (void(fg::Universe::*) (boost::shared_ptr<MeshNode>)) &fg::Universe::add)
		   .def("add", (void(fg::Universe::*) (boost::shared_ptr<Node>)) &fg::Universe::add)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)
		   .property("t", &fg::Universe::time)
		   .def("time", &fg::Universe::time)
		];

		// fg/functions.h
		module(L,"fg")[
		   def("min", &min<double>),
		   def("lerp", &lerp<double, double>),
           def("mix", &mix<double, double>),
           def("clamp", &clamp<double>),
           def("step", &step<double, double>),
           def("pulse", &pulse<double, double>),
           def("smoothstep", &smoothstep<double>),
           def("catmullSpline", &catmullSpline<double>),
           def("bias", &fg::bias<double>),
           def("gain", &fg::gain<double>),
           def("gamma", &gammaCorrect<double>),
           def("invSqrt", &invSqrt<double>),
           def("sqr", &sqr<double>),
           def("sign", &sign<double>),

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

		// fg/mat4.h
		module(L, "fg")[
		   class_<fg::Mat4>("mat4")
		   .def(constructor<>())
		   .def(tostring(const_self))

		   .def("setTranslate", (void(fg::Mat4::*)(double,double,double)) &fg::Mat4::setTranslate)
		   .def("setRotateRad", (void(fg::Mat4::*)(double,double,double,double)) &fg::Mat4::setRotateRad)

		   .def(const_self + other<fg::Mat4>())
		   .def(const_self - other<fg::Mat4>())
		   .def(const_self * double())
		];

		// fg/node.h
		module(L, "fg")[
		  class_<fg::Node, boost::shared_ptr<fg::Node> >("node")
		  .def(constructor<>())
		  .property("transform", &fg::Node::getRelativeTransform)
		];

		// fg/meshnode.h
		module(L, "fg")[
		   class_<fg::MeshNode, fg::Node, boost::shared_ptr<fg::MeshNode> >("meshnode")
		   .def(constructor<boost::shared_ptr<fg::Mesh> >())
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

		// fg/turtle.h
		module(L,"fg")[
		   class_<fg::gc::Turtle>("turtle")
		   .def(constructor<>())
		   .def("move",(void(fg::gc::Turtle::*)(double))&fg::gc::Turtle::move)
		   .def("yaw",(void(fg::gc::Turtle::*)(double))&fg::gc::Turtle::yaw)
		   .def("roll",(void(fg::gc::Turtle::*)(double))&fg::gc::Turtle::roll)
		   .def("pitch",(void(fg::gc::Turtle::*)(double))&fg::gc::Turtle::pitch)
		   .def("beginCylinder",&fg::gc::Turtle::beginCylinder)
		   .def("endCylinder",&fg::gc::Turtle::endCylinder)
		   .def("addPoint",&fg::gc::Turtle::addPoint)
		   .def("getMesh",(boost::shared_ptr<Mesh>(fg::gc::Turtle::*)(int,int,int))&fg::gc::Turtle::getMesh)
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
