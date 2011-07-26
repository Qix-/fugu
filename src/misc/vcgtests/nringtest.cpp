#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>

#include "GL/glew.h"
#include "GL/glfw.h"
// #include <GL/gl.h>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/math/perlin_noise.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>

#include <vcg/complex/algorithms/smooth.h>

#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/extended_marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/color.h>

#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/complex/algorithms/refine.h>

// for selection/walking
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/algorithms/nring.h>

// for opengl
#include <wrap/gl/trimesh.h>

using namespace std;
using namespace vcg;

typedef float ScalarType;

class MyFace;
class MyVertex;

struct MyUsedTypes : public UsedTypes<
Use<MyVertex>	::AsVertexType,
Use<MyFace>		::AsFaceType>{};

// NB: Need vertex::VFAdj AND face::VFAdj for VF adjacency
class MyVertex: public Vertex< MyUsedTypes, vertex::Color4b, vertex::Coord3f, vertex::Normal3f, vertex::BitFlags, vertex::VFAdj, vertex::InfoOcf, vertex::Mark>{};
class MyFace: public Face< MyUsedTypes, face::FFAdj, face::VFAdj, face::Mark, face::VertexRef, face::Normal3f, face::BitFlags, face::InfoOcf> {};
class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

typedef SimpleVolume<SimpleVoxel> MyVolume;

void error(const char* msg){
	std::cerr << msg;
	exit(-1);
}

void error(const char* msg, const char* args){
	fprintf(stderr, msg, args);
	exit(-1);
}

int DRAW_MODE = 2;
const int NUM_DRAW_MODES = 5;

void GLFWCALL keyCallback(int key, int action)
{
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		DRAW_MODE = (DRAW_MODE + 1)%NUM_DRAW_MODES;
	}
}

void setupGL();

double _random(){
	return 1.0*rand()/RAND_MAX;
}

double _random(double low, double high){
	return _random()*(high-low) + low;
}

int main(int argc, char *argv[])
{
	setupGL();
	srand(time(NULL));

	// MARCHING CUBES
	MyMesh mesh;
	vcg::tri::Dodecahedron<MyMesh>(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);

	for(int i=0;i<4;i++)
		Refine(mesh,MidPoint<MyMesh>(&mesh));

	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);

	// Test the topology (only for 2-manifolds!)
	vcg::tri::UpdateTopology<MyMesh>::TestVertexFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::TestFaceFace(mesh);

	// make white
	vcg::tri::UpdateColor<MyMesh>::VertexConstant(mesh,Color4b::White);

	/*
	MyMesh::FaceIterator it = mesh.face.begin();
	for (;it!=mesh.face.end();++it){
		it->C()[0] = 255;
		it->C()[1] = 255;
		it->C()[2] = 255; //(unsigned char) (255*_random());
	}
	*/

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

	// Or we can use the n-ring helper class
	Color4b rand;
	rand[0] = (unsigned char)(_random()*255);
	rand[1] = (unsigned char)(_random()*255);
	rand[2] = (unsigned char)(_random()*255);

	for(int i=0;i<20;i++){
		vcg::tri::Nring<MyMesh>::clearFlags(&mesh); // Probably necessary..
		vcg::tri::Nring<MyMesh> n(&mesh.vert[(int)(_random()*(mesh.vert.size()-1))],&mesh);
		int sz = (int) _random(1,10);
		n.expand(sz);

		double dt = 1./n.lastV.size();
		double t = dt;
		BOOST_FOREACH(MyMesh::VertexType* v, n.lastV)
		{
			unsigned char c = (unsigned char)(t*255);
			v->C() = Color4b(c,0,255-c,255);
			t += dt;
		}
	}

	/*
	BOOST_FOREACH(MyMesh::FaceType* f, n.allF)
	{
		f->C() = rand;
	}

	BOOST_FOREACH(MyMesh::FaceType* f, n.lastF)
	{
		f->C() = Color4b::Black;
	}*/

	GlTrimesh<MyMesh> tm;
	tm.m = &mesh;
	tm.Update();

	float rotate = 0;
	vcg::Point3<float> c = mesh.bbox.Center();
	float meshSize = mesh.bbox.DimY();

	int running = GL_TRUE;

	while(running){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double now = glfwGetTime();



		tm.Update();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(.7,.7,1.7,   0,0,0,   0,1,0);

		GLfloat lp[] = {.1,1,.2,0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		glRotatef(now*20,0,1,0);
		glScalef(1/meshSize,1/meshSize,1/meshSize);

		glTranslatef(-c.X(),-c.Y(),-c.Z());

		glColor3f(1,1,1);
		switch (DRAW_MODE){
		case 0: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMNone> ();	break;
		case 1: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerVert,vcg::GLW::TMNone> (); break;
		case 2: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerVert, vcg::GLW::TMNone> (); break;
		case 3: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerVert,vcg::GLW::TMNone> (); break;
		case 4: tm.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMPerVert,vcg::GLW::TMNone> (); break;
		}
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		const double SPF = 20/1000.0;
		double dt = glfwGetTime() - now;
		if (dt < SPF) glfwSleep(SPF - dt); // maintain 1.0/SPF frames per second
		// std::cout << dt*1000 << ", slept for " << (SPF - dt)*1000 << "\n";
	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
};

void setupGL(){
	int width = 800;
	int height = 800;

	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	if (!glfwOpenWindow(width,height, 0,0,0,0,8,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(keyCallback);

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glClearColor(0, 0, 0, 1);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat amb[] = {0,0,0,1};
	GLfloat diff[] = {1,1,1,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diff);
	/*
		GLfloat global_ambient[] = { 0.3f, 0.0f, 0.3f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);

		GLfloat col[] = {1.f,0.f,1.f,0.f};
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);
		GLfloat white[] = {1.f,1.f,1.f,0.f};
		GLfloat spec[] = {.5f,.5f,.5f,0.f};
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
	 */

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glColor3f(1,0,1);
	glLoadIdentity();
	gluPerspective(40, 1.0*width/height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);

}
