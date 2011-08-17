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
#include <cmath>

#include "fg/vec3.h"
#include "fg/mat4.h"
#include "fg/universe.h"
#include "fg/functions.h"
#include "fg/node.h"
#include "fg/mesh.h"
#include "fg/vertex.h"
#include "fg/face.h"
#include "fg/meshnode.h"
#include "fg/meshoperators.h"
#include "fg/pos.h"

#include "fg/gc/turtle.h"

int debugFileAndLine(lua_State* L);

// hack
fg::Vec3 matmul(const fg::Mat4& m, const fg::Vec3& v){
	return m*((vcg::Point3<double>)(v));
}

namespace fg {
	int loadLuaBindings(lua_State* L){
		using namespace luabind;

		//set_pcall_callback(debugFileAndLine);
		open(L);

		// All functions and math types populate the global namespace
		// NB: If this causes problems, well ... we'll change it I guess

		// fg/functions.h
		module(L)[
		   def("min", &min<double>),
		   def("lerp", &lerp<double, double>),
           def("mix", &mix<double, double>),
           def("clamp", &clamp<double>),
           def("step", &step<double, double>),
           def("pulse", &pulse<double, double>),
           def("smoothstep", &smoothstep<double>),
           // def("catmullSpline", &catmullSpline<double>), can't use this without adapting the array (BP)
           def("bias", &fg::bias<double>),
           def("gain", &fg::gain<double>),
           def("gamma", &gammaCorrect<double>),
           def("invSqrt", &invSqrt<double>),
           def("sqrt", (double(*)(double)) &std::sqrt),
           def("sqr", &sqr<double>),
           def("sign", &sign<double>),

           def("noise", (double(*)(double)) &noise),
		   def("noise", (double(*)(double,double)) &noise),
		   def("noise", (double(*)(double,double,double)) &noise),
                       
           def("fracSum", (double(*)(double,double,double,int,double))&fracSum),
           def("turbulence", (double(*)(double,double,double,int,double))&turbulence),

		   def("random", (double(*)()) &fg::random),
		   def("random", (double(*)(double,double)) &fg::random),
           def("randomN", (double(*)(double,double))&randomN)
		   ];


		/*
		module(L)[
		          class_<vcg::Point3<double> >("point3d")
		];*/

		// fg/vec3.h
		module(L)[
		   class_<fg::Vec3>("vec3")
		   .def(constructor<>())
		   .def(constructor<double,double,double>())
		   .def(constructor<const vcg::Point3d&>())
		   .property("x", &fg::Vec3::getX, &fg::Vec3::setX)
		   .property("y", &fg::Vec3::getY, &fg::Vec3::setY)
		   .property("z", &fg::Vec3::getZ, &fg::Vec3::setZ)

		   // can also access like a colour
		   .property("r", &fg::Vec3::getX, &fg::Vec3::setX)
		   .property("g", &fg::Vec3::getY, &fg::Vec3::setY)
		   .property("b", &fg::Vec3::getZ, &fg::Vec3::setZ)

		   .def(tostring(const_self))

		   // operators
		   .def(const_self + other<fg::Vec3>())
		   .def(const_self - other<fg::Vec3>())
		   .def(const_self * double())
		   .def(const_self / double())
		   .def(-(const_self))

		   .def(const_self == other<fg::Vec3>())

		   // methods
		   .def("normalise",&fg::Vec3::normalise),

		   // vec3 free functions (after GLSL Geometric Functions)
		   def("length",(void(*)(const Vec3&)) &fg::length),
		   def("distance",(void(*)(const Vec3&,const Vec3&)) &fg::distance),
		   def("dot",(void(*)(const Vec3&,const Vec3&)) &fg::dot),
		   def("cross",(Vec3(*)(const Vec3&,const Vec3&)) &fg::cross),
		   def("normalise",(Vec3(*)(const Vec3&)) &fg::normalise)
		];


		// fg/mat4.h
		module(L)[
		   class_<fg::Mat4>("mat4")
		   .def(constructor<>())
		   /* // Would like to do this but luabind can only take up to 10 params
		   .def(constructor<double,double,double,double,
				   double,double,double,double,
				   double,double,double,double,
				   double,double,double,double>())
			*/
		   .def(tostring(const_self))

		   // operators
		   .def(const_self + other<fg::Mat4>())
		   .def(const_self - other<fg::Mat4>())
		   .def(-(const_self))
		   .def(const_self * double())
		   .def(const_self * other<fg::Mat4>())
		   .def("__mul", (fg::Vec3(*)(const fg::Mat4&, const fg::Vec3&))&matmul)
		   // .def(const_self * other<fg::Vec3>()) // free function
		   .def(const_self == other<fg::Mat4>())

		   // methods
		   .def("get", (double(fg::Mat4::*)(int,int)const) &fg::Mat4::get)

		   // transform methods
		   .def("set", (void(fg::Mat4::*)(const Vec3&, const Vec3&, const Vec3&)) &fg::Mat4::set)
		   .def("setBasis", (Mat4&(fg::Mat4::*)(const Vec3&, const Vec3&, const Vec3&)) &fg::Mat4::set)
		   .def("setTranslate", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setTranslate)
		   .def("setTranslate", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setTranslate)
		   .def("setRotateRad", (Mat4&(fg::Mat4::*)(double,double,double,double)) &fg::Mat4::setRotateRad)
		   .def("setRotateRad", (Mat4&(fg::Mat4::*)(double,const Vec3&)) &fg::Mat4::setRotateRad)
		   .def("setRotate",  (Mat4&(fg::Mat4::*)(const Vec3&,const Vec3&)) &fg::Mat4::setRotate)
		   .def("setScale", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setScale)
		   .def("setScale", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setScale)
		];

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

		// fg/node.h
		module(L, "fg")[
		  class_<fg::Node, boost::shared_ptr<fg::Node> >("node")
		  .def(constructor<>())

		  .def("getTransform", &fg::Node::getRelativeTransform)
		  .def("setTransform", &fg::Node::setRelativeTransform)

		  // .property("transform", &fg::Node::getRelativeTransform, &fg::Node::setRelativeTransform)
		];

		// fg/meshnode.h
		module(L, "fg")[
		   class_<fg::MeshNode, fg::Node, boost::shared_ptr<fg::MeshNode> >("meshnode")
		   .def(constructor<boost::shared_ptr<fg::Mesh> >())
		   .def("setMesh", &fg::MeshNode::setMesh)
		];

		// fg/vertex.h
		module(L,"fg")[
		   class_<fg::VertexProxy>("vertex")
		   .def(tostring(const_self))

		   // Properties
		   .def("getPos", &fg::VertexProxy::getPos)
		   .def("setPos", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))
		   .def("setPos", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setPos))
		   .property("p", &fg::VertexProxy::getPos, (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))

		   .def("getN", &fg::VertexProxy::getN)
		   // class_<fg::Mesh::VertexContainer>("vertexcontainer"),

		   .def("getColour", &fg::VertexProxy::getColour)
		   .def("setColour", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setColour))
		   .def("setColour", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setColour))

		   .def("setUV", &fg::VertexProxy::setUV)

		   .def("getAdjacentFace", &fg::VertexProxy::getAdjacentFace)

		   .property("valid", &fg::VertexProxy::isValid)
		];

		// fg/face.h
		module(L,"fg")[
		   class_<fg::FaceProxy>("face")
		   .def(tostring(const_self))

		   .property("n", &fg::FaceProxy::getN)
		   .def("v", &fg::FaceProxy::getV)

		   .property("valid", &fg::FaceProxy::isValid)

		   .def(const_self == other<fg::FaceProxy>())
		];

		// fg/pos.h
		module(L,"fg")[

		  class_<fg::Pos>("pos")
		  .def(constructor<boost::shared_ptr<fg::FaceProxy>,int,boost::shared_ptr<fg::VertexProxy> >())
		  .def(tostring(const_self))

		  .def("flipV", &fg::Pos::flipV)
		  .def("flipE", &fg::Pos::flipE)
		  .def("flipF", &fg::Pos::flipF)

		  .property("v", &fg::Pos::getV)
		  .property("e", &fg::Pos::getE)
		  .property("f", &fg::Pos::getF)
		];

		// fg/mesh.h
		module(L,"fg")[
		   // containers
		   class_<fg::Mesh::VertexSet>("vertexset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::VertexSet>, return_stl_iterator),

		   class_<fg::Mesh::FaceSet>("faceset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::FaceSet>, return_stl_iterator),

		   // mesh
		   class_<fg::Mesh>("mesh")
		   .def(tostring(const_self))

		   // Selectors
		   .def("selectAllVertices", &Mesh::selectAllVertices)
		   .def("selectAllFaces", &Mesh::selectAllFaces)

		   .def("selectRandomVertex", &Mesh::selectRandomVertex)
		   .def("selectRandomFace", &Mesh::selectRandomFace)

		   .def("subdivide", &Mesh::subdivide)
		   .def("smoothSubdivide", &Mesh::smoothSubdivide)
		   .def("sync", &Mesh::sync)
		   .def("applyTransform", &Mesh::applyTransform)

		   .scope [
			   class_<fg::Mesh::Primitives>("primitives")
			   .scope [
			           def("cube",&fg::Mesh::Primitives::Cube),
					   def("icosahedron",&fg::Mesh::Primitives::Icosahedron),
					   def("sphere",&fg::Mesh::Primitives::Sphere),
					   def("tetrahedron", &fg::Mesh::Primitives::Tetrahedron),
					   def("dodecahedron", &fg::Mesh::Primitives::Dodecahedron),
					   def("octahedron", &fg::Mesh::Primitives::Octahedron),
					   def("cone", &fg::Mesh::Primitives::Cone), // (r1,r2,subdiv=36)
					   def("cylinder", &fg::Mesh::Primitives::Cylinder) // (slices) // ,stacks)
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

		   .def("setFrame",(void(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrame)
		   .def("setFrameRel",(void(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrameRel)
		   .def("setStiffness",(void(fg::gc::Turtle::*)(double,double))&fg::gc::Turtle::setStiffness)
		   .def("setScale",(void(fg::gc::Turtle::*)(double))&fg::gc::Turtle::setScale)
		   .def("setCrossSection",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCrossSection)
		   .def("pushState",&fg::gc::Turtle::pushState)
		   .def("popState",&fg::gc::Turtle::popState)
		   .def("setCarrierMode",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCarrierMode)

		   .def("beginCrossSection",&fg::gc::Turtle::beginCrossSection)
		   .def("endCrossSection",(int(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCrossSection)

		   .def("beginCylinder",&fg::gc::Turtle::beginCylinder)
		   .def("endCylinder",(void(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCylinder)
		   .def("endCylinder",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::endCylinder)
		   .def("addPoint",(void(fg::gc::Turtle::*)(void))&fg::gc::Turtle::addPoint)
		   .def("addPoint",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::addPoint)

		   .def("getPosition",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getPosition)
		   .def("getMesh",(boost::shared_ptr<Mesh>(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getMesh)

		   .def("getNumEndCaps",&fg::gc::Turtle::getNumEndCaps)
		   .def("getEndCap",&fg::gc::Turtle::getEndCap)
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
