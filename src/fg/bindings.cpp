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
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
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
#include "fg/quat.h"
#include "fg/universe.h"
#include "fg/functions.h"
#include "fg/node.h"
#include "fg/mesh.h"
#include "fg/vertex.h"
#include "fg/face.h"
#include "fg/meshnode.h"
#include "fg/meshoperators.h"
#include "fg/pos.h"
#include "fg/phyllo.h"
#include "fg/geometry_wrapper.h"

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
		   // def("min", &min<double>), // use lua's standard min

		   def("lerp", &lerp<double, double>),
		   def("lerp", &lerp<Vec3, double>),
		   def("slerp", &slerp),

           def("mix", &mix<double, double>),
           def("clamp", &clamp<double>),
           def("step", &step<double, double>),
           def("pulse", &pulse<double, double>),
           def("smoothstep", &smoothstep<double>),
           // def("catmullSpline", &catmullSpline<double>), can't use this without adapting the array (BP)
           def("bias", &fg::bias<double>),
           def("gain", &fg::gain<double>),
           def("gamma", &gammaCorrect<double>),
           def("inv_sqrt", &invSqrt<double>),
           // def("sqrt", (double(*)(double)) &std::sqrt), // use luas std sqrt
           def("sqr", &sqr<double>),
           def("sign", &sign<double>),

           def("noise", (double(*)(double)) &noise),
		   def("noise", (double(*)(double,double)) &noise),
		   def("noise", (double(*)(double,double,double)) &noise),
                       
           def("frac_sum", (double(*)(double,double,double,int,double))&fracSum),
           def("turbulence", (double(*)(double,double,double,int,double))&turbulence),

		   def("random", (double(*)()) &fg::random),
		   def("random", (double(*)(double,double)) &fg::random),
           def("randomN", (double(*)(double,double))&randomN), // TODO: deprecate
           def("gauss", (double(*)(double,double))&randomN),
                  
           def("get_ellipse_area", (double(*)(double,double,double))&getEllipseArea),
           def("find_ellipse_h", (double(*)(double,double,double))&findEllipseH),
           def("get_angle_from_h", (double(*)(double,double,double))&getAngleFromH)
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
		   .def(const_self * other<fg::Mat4>())
		   .def(const_self / double())
		   .def(-(const_self))

		   .def(const_self == other<fg::Vec3>())

		   // methods
		   .def("normalise",&fg::Vec3::normalise),

		   // vec3 free functions (after GLSL Geometric Functions)
		   def("length",(double(*)(const Vec3&)) &fg::length),
		   def("distance",(double(*)(const Vec3&,const Vec3&)) &fg::distance),
		   def("dot",(double(*)(const Vec3&,const Vec3&)) &fg::dot),
		   def("cross",(Vec3(*)(const Vec3&,const Vec3&)) &fg::cross),
		   def("normalise",(Vec3(*)(const Vec3&)) &fg::normalise)
		];


		// fg/mat4.h
		module(L)[
		   class_<fg::Mat4>("mat4")
		   .def(constructor<>())
		   /*// TODO: Would like to do this but luabind can only take up to 10 params (need to change the #defines in luabind)
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
		   .def("set_basis", (Mat4&(fg::Mat4::*)(const Vec3&, const Vec3&, const Vec3&)) &fg::Mat4::set)
		   .def("set_translate", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setTranslate)
		   .def("set_translate", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setTranslate)
		   .def("set_rotate_rad", (Mat4&(fg::Mat4::*)(double,double,double,double)) &fg::Mat4::setRotateRad)
		   .def("set_rotate_rad", (Mat4&(fg::Mat4::*)(double,const Vec3&)) &fg::Mat4::setRotateRad)
		   .def("set_rotate",  (Mat4&(fg::Mat4::*)(const Vec3&,const Vec3&)) &fg::Mat4::setRotate)
		   .def("set_scale", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setScale)
		   .def("set_scale", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setScale)
		   .def("set_scale", (Mat4&(fg::Mat4::*)(double)) &fg::Mat4::setScale)

		   // TODO: deprecate this syntax style
		   .def("setBasis", (Mat4&(fg::Mat4::*)(const Vec3&, const Vec3&, const Vec3&)) &fg::Mat4::set)
		   .def("setTranslate", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setTranslate)
		   .def("setTranslate", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setTranslate)
		   .def("setRotateRad", (Mat4&(fg::Mat4::*)(double,double,double,double)) &fg::Mat4::setRotateRad)
		   .def("setRotateRad", (Mat4&(fg::Mat4::*)(double,const Vec3&)) &fg::Mat4::setRotateRad)
		   .def("setRotate",  (Mat4&(fg::Mat4::*)(const Vec3&,const Vec3&)) &fg::Mat4::setRotate)
		   .def("setScale", (Mat4&(fg::Mat4::*)(double,double,double)) &fg::Mat4::setScale)
		   .def("setScale", (Mat4&(fg::Mat4::*)(const Vec3&)) &fg::Mat4::setScale)
		   .def("setScale", (Mat4&(fg::Mat4::*)(double)) &fg::Mat4::setScale)
		];

		// fg/quat.h
		module(L)[
		   class_<fg::Quat>("quat")
		   .def(constructor<>())
		   .def(constructor<double, double, double, double>())
		   .def(constructor<const Vec3&, double>())
		   .def(constructor<const Vec3&, const Vec3&>())
		   .def(constructor<double,double,double>())
		   .def(constructor<const Mat4&>())
		   .def(constructor<const Vec3&, const Vec3&, const Vec3&>())
		   .def(tostring(const_self))

		   // operators
		   .def(const_self * other<const fg::Quat&>())

		   /*
		   .def(const_self + other<const fg::Quat&>())
		   .def(const_self - other<const fg::Quat&>())
		   */

		   .def(const_self * double())
		   .def(const_self * other<const fg::Vec3&>())
		];

		// fg/universe.h
		module(L,"fg")[
		   class_<fg::Universe>("universe")
		   .def(tostring(const_self))
		   .def("addMesh", &fg::Universe::addMesh) // deprecated..
		   .def("add_mesh", &fg::Universe::addMesh) // deprecated..
		   .def("add", (void(fg::Universe::*) (boost::shared_ptr<MeshNode>)) &fg::Universe::add)
		   .def("add", (void(fg::Universe::*) (boost::shared_ptr<Node>)) &fg::Universe::add)

		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("makeChildOf", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)

		   .def("make_child_of", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)
		   .def("make_child_of", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("make_child_of", (void(fg::Universe::*)(boost::shared_ptr<Node> parent, boost::shared_ptr<MeshNode>)) &fg::Universe::makeChildOf)
		   .def("make_child_of", (void(fg::Universe::*)(boost::shared_ptr<MeshNode> parent, boost::shared_ptr<Node>)) &fg::Universe::makeChildOf)


		   .property("t", &fg::Universe::time)
		   .def("time", &fg::Universe::time)
		];

		// fg/node.h
		module(L, "fg")[
		  class_<fg::Node, boost::shared_ptr<fg::Node> >("node")
		  .def(constructor<>())
		  .def(tostring(const_self))

		  .def("getTransform", &fg::Node::getRelativeTransform)
		  .def("setTransform", &fg::Node::setRelativeTransform)

		  .def("get_transform", &fg::Node::getRelativeTransform)
		  .def("set_transform", &fg::Node::setRelativeTransform)

		  // .property("transform", &fg::Node::getRelativeTransform, &fg::Node::setRelativeTransform)
		];

		// fg/meshnode.h
		module(L, "fg")[
		   class_<fg::MeshNode, fg::Node, boost::shared_ptr<fg::MeshNode> >("meshnode")
		   .def(constructor<boost::shared_ptr<fg::Mesh> >())
		   .def(tostring(const_self))

		   .def("setMesh", &fg::MeshNode::setMesh)
		   .def("set_mesh", &fg::MeshNode::setMesh)
		];

		// fg/vertex.h
		module(L,"fg")[
		   class_<fg::VertexProxy>("vertex")
		   .def(tostring(const_self))

		   // Properties
		   //TODO: deprecate old style
		   .def("getPos", &fg::VertexProxy::getPos)
		   .def("setPos", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))
		   .def("setPos", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setPos))
		   .def("get_pos", &fg::VertexProxy::getPos)
		   .def("set_pos", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))
		   .def("set_pos", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setPos))

		   .property("p", &fg::VertexProxy::getPos, (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setPos))

		   .def("getN", &fg::VertexProxy::getN)
		   .def("get_n", &fg::VertexProxy::getN)

		   .property("n", &fg::VertexProxy::getN)
		   .def("calculateNormal", &fg::VertexProxy::calculateNormal)
		   .def("calculate_normal", &fg::VertexProxy::calculateNormal)
		   // class_<fg::Mesh::VertexContainer>("vertexcontainer"),

		   .def("getColour", &fg::VertexProxy::getColour)
		   .def("setColour", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setColour))
		   .def("setColour", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setColour))

		   .def("get_colour", &fg::VertexProxy::getColour)
		   .def("set_colour", (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setColour))
		   .def("set_colour", (void(fg::VertexProxy::*)(double,double,double)) (&fg::VertexProxy::setColour))

		   .property("c", &fg::VertexProxy::getColour, (void(fg::VertexProxy::*)(fg::Vec3)) (&fg::VertexProxy::setColour))

		   .def("setUV", &fg::VertexProxy::setUV)
		   .def("set_uv", &fg::VertexProxy::setUV)

		   .def("getAdjacentFace", &fg::VertexProxy::getAdjacentFace)
		   .def("get_adjacent_face", &fg::VertexProxy::getAdjacentFace)

		   .property("valid", &fg::VertexProxy::isValid)

		   .def(const_self == other<fg::VertexProxy>())
		];

		// fg/face.h
		module(L,"fg")[
		   class_<fg::FaceProxy>("face")
		   .def(tostring(const_self))

		   // TODO: deprecate old syntax
		   .def("getN", &fg::FaceProxy::getN)
		   .def("get_n", &fg::FaceProxy::getN)

		   .property("n", &fg::FaceProxy::getN)
		   .def("calculateNormal", &fg::FaceProxy::calculateNormal)
		   .def("calculate_normal", &fg::FaceProxy::calculateNormal)

		   .def("v", &fg::FaceProxy::getV)
		   .def("getV", &fg::FaceProxy::getV)

		   .property("valid", &fg::FaceProxy::isValid)

		   .def(const_self == other<fg::FaceProxy>())
		];

		// fg/pos.h
		module(L,"fg")[

		  class_<fg::Pos>("pos")
		  .def(constructor<boost::shared_ptr<fg::FaceProxy>,int,boost::shared_ptr<fg::VertexProxy> >())
		  .def(constructor<boost::shared_ptr<fg::FaceProxy>,boost::shared_ptr<fg::VertexProxy> >())
		  .def(constructor<const Pos&>())
		  .def(tostring(const_self))

		  // TODO: deprecate old sytnax
		  .def("flipV", &fg::Pos::flipV)
		  .def("flipE", &fg::Pos::flipE)
		  .def("flipF", &fg::Pos::flipF)

		  .def("flip_v", &fg::Pos::flipV)
		  .def("flip_e", &fg::Pos::flipE)
		  .def("flip_f", &fg::Pos::flipF)

		  .property("v", &fg::Pos::getV)
		  .property("e", &fg::Pos::getE)
		  .property("f", &fg::Pos::getF)

		  .def(const_self == other<fg::Pos>())
		];

		// fg/mesh.h
		module(L,"fg")[
		   // containers
		   class_<fg::Mesh::VertexSet>("vertexset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::VertexSet>, return_stl_iterator)
		   .property("verts", &fg::luaAllAdapter<fg::Mesh::VertexSet>, return_stl_iterator),

		   class_<fg::Mesh::FaceSet>("faceset")
		   .property("all", &fg::luaAllAdapter<fg::Mesh::FaceSet>, return_stl_iterator)
		   .property("faces", &fg::luaAllAdapter<fg::Mesh::FaceSet>, return_stl_iterator),

		   // mesh
		   class_<fg::Mesh>("mesh")
		   .def(tostring(const_self))

		   // Selectors
		   // Move selectors to free functions
		   .def("selectAllVertices", &Mesh::selectAllVertices)
		   .def("selectAllFaces", &Mesh::selectAllFaces)

		   .def("selectRandomVertex", &Mesh::selectRandomVertex)
		   .def("selectRandomFace", &Mesh::selectRandomFace)

		   .def("select_all_vertices", &Mesh::selectAllVertices)
		   .def("select_all_faces", &Mesh::selectAllFaces)

		   .def("select_random_vertex", &Mesh::selectRandomVertex)
		   .def("select_random_face", &Mesh::selectRandomFace)

		   .def("subdivide", &Mesh::subdivide)
		   .def("smoothSubdivide", &Mesh::smoothSubdivide) // TODO: deprecate
		   .def("smooth_subdivide", &Mesh::smoothSubdivide)
		   .def("sync", &Mesh::sync)
		   .def("apply_transform", &Mesh::applyTransform) // TODO: deprecate
		   .def("applyTransform", &Mesh::applyTransform)
		   .def("clone", &Mesh::clone)

		   .scope [
		           // TODO: these have been moved to global namespace
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

		module(L)[
			def("cube",&fg::Mesh::Primitives::Cube),
			def("icosahedron",&fg::Mesh::Primitives::Icosahedron),
			def("sphere",&fg::Mesh::Primitives::Sphere),
			def("tetrahedron", &fg::Mesh::Primitives::Tetrahedron),
			def("dodecahedron", &fg::Mesh::Primitives::Dodecahedron),
			def("octahedron", &fg::Mesh::Primitives::Octahedron),
			def("cone", &fg::Mesh::Primitives::Cone), // (r1,r2,subdiv=36)
			def("cylinder", &fg::Mesh::Primitives::Cylinder), // (slices) // ,stacks)
			def("iso", (shared_ptr<fg::Mesh>(*)(int,luabind::object)) &fg::Mesh::Primitives::Iso)
		];

		// fg/meshoperators.h
		module(L,"fg")[
		   def("_extrude", (void(*)(Mesh*,VertexProxy,double))&fg::extrude),
		   def("_extrude", (void(*)(Mesh*,VertexProxy,int,Vec3,double))&fg::extrude),
		   def("getVerticesAtDistance", getVerticesAtDistance),
		   def("getVerticesWithinDistance", getVerticesWithinDistance),
		   def("nloop", nloop),

		   def("splitEdge", splitEdge),
		   def("split_edge", splitEdge),

		   /// \deprecated
		   def("_extrude", (void(*)(Mesh*,VertexProxy,int,Vec3,double,double))&fg::extrude)
		];

		// fg/turtle.h
		module(L,"fg")[
		   class_<fg::gc::Turtle>("turtle")
		   .def(constructor<>())
		   .def("move",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::move)
		   .def("yaw",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::yaw)
		   .def("roll",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::roll)
		   .def("pitch",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::pitch)

		   .def("setFrame",(fg::gc::Turtle&(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrame)
		   .def("setFrameRel",(fg::gc::Turtle&(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrameRel)
		   .def("setStiffness",(fg::gc::Turtle&(fg::gc::Turtle::*)(double,double))&fg::gc::Turtle::setStiffness)
		   .def("setScale",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::setScale)
		   .def("setCrossSection",(fg::gc::Turtle&(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCrossSection)

		   .def("set_frame",(fg::gc::Turtle&(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrame)
		   .def("set_frame_rel",(fg::gc::Turtle&(fg::gc::Turtle::*)(Vec3,Vec3,Vec3))&fg::gc::Turtle::setFrameRel)
		   .def("set_stiffness",(fg::gc::Turtle&(fg::gc::Turtle::*)(double,double))&fg::gc::Turtle::setStiffness)
		   .def("set_scale",(fg::gc::Turtle&(fg::gc::Turtle::*)(double))&fg::gc::Turtle::setScale)
		   .def("set_cross_section",(fg::gc::Turtle&(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCrossSection)

		   .def("pushState",&fg::gc::Turtle::pushState)
		   .def("popState",&fg::gc::Turtle::popState)

		   .def("push",&fg::gc::Turtle::pushState)
		   .def("pop",&fg::gc::Turtle::popState)

		   .def("setCarrierMode",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCarrierMode)
		   .def("set_carrier_mode",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::setCarrierMode)

		   .def("beginCrossSection",&fg::gc::Turtle::beginCrossSection)
		   .def("endCrossSection",(int(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCrossSection)

		   .def("begin_cross_section",&fg::gc::Turtle::beginCrossSection)
		   .def("end_cross_section",(int(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCrossSection)

		   .def("beginCylinder",&fg::gc::Turtle::beginCylinder)
		   .def("endCylinder",(void(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCylinder)
		   .def("endCylinder",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::endCylinder)
		   .def("addPoint",(fg::gc::Turtle&(fg::gc::Turtle::*)(void))&fg::gc::Turtle::addPoint)
		   .def("addPoint",(fg::gc::Turtle&(fg::gc::Turtle::*)(int))&fg::gc::Turtle::addPoint)

		   .def("begin_cylinder",&fg::gc::Turtle::beginCylinder)
		   .def("end_cylinder",(void(fg::gc::Turtle::*)(void))&fg::gc::Turtle::endCylinder)
		   .def("end_cylinder",(void(fg::gc::Turtle::*)(int))&fg::gc::Turtle::endCylinder)
		   .def("add_point",(fg::gc::Turtle&(fg::gc::Turtle::*)(void))&fg::gc::Turtle::addPoint)
		   .def("add_point",(fg::gc::Turtle&(fg::gc::Turtle::*)(int))&fg::gc::Turtle::addPoint)

		   .def("getPosition",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getPosition)
		   .def("getHeading",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getHeading)
		   .def("getUp",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getUp)
		   .def("getFrame",(fg::Mat4(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getFrame)
		   .def("get_position",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getPosition)
		   .def("get_heading",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getHeading)
		   .def("get_up",(fg::Vec3(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getUp)
		   .def("get_frame",(fg::Mat4(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getFrame)

		   .def("getMesh",(boost::shared_ptr<Mesh>(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getMesh)
		   .def("mesh",(boost::shared_ptr<Mesh>(fg::gc::Turtle::*)(void))&fg::gc::Turtle::getMesh)

		   .def("getNumEndCaps",&fg::gc::Turtle::getNumEndCaps)
		   .def("getEndCap",&fg::gc::Turtle::getEndCap)

		   .def("getNumGC",&fg::gc::Turtle::getNumGC)
		   .def("getMesh",(boost::shared_ptr<Mesh>(fg::gc::Turtle::*)(int))&fg::gc::Turtle::getMesh)


		];

		// fg/geometry_wrapper.h
		module(L)[
		    def("line_exp_point_dist",line_exp_point_dist_3d),
		    def("segment_point_dist",segment_point_dist_3d)
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
