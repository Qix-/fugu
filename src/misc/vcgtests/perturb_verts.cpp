#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>

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

#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/complex/algorithms/refine.h>

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

// NOTE: Need Normal3f or will assert!
//class MyVertex: public Vertex< MyUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::BitFlags>{};
// class MyFace: public Face< MyUsedTypes, face::InfoOcf, face::FFAdjOcf, face::VertexRef, face::Normal3f, face::BitFlags> {};
// class MyFace: public Face< MyUsedTypes, face::InfoOcf, face::FFAdjOcf, face::VertexRef, face::Normal3f, face::BitFlags> {};
//class MyFace: public Face< MyUsedTypes, face::VertexRef, face::Normal3f, face::BitFlags> {};
//class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

class MyVertex: public Vertex< MyUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::BitFlags, vertex::VFAdj, vertex::InfoOcf, vertex::Mark>{};
class MyFace: public Face< MyUsedTypes, face::FFAdj, face::Mark, face::VertexRef, face::Normal3f, face::BitFlags, face::InfoOcf> {};
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

int DRAW_MODE = 2; // start in flat
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
	vcg::tri::Icosahedron<MyMesh>(mesh);

	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	for(int i=0;i<5;i++)
		Refine(mesh,MidPoint<MyMesh>(&mesh));

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

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

		MyMesh::VertexIterator it = mesh.vert.begin();

		for (;it!=mesh.vert.end();++it){
			double p[3] = {it->P()[0],it->P()[1],it->P()[2]};
			double val = math::Perlin::Noise(p[0],p[1],p[2]+now);
			for (int i=0;i<3;i++)
				it->P()[i] += val*_random(0,.01);
		}

		// then smooth the positions a bit
		// tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mesh,1);

		vcg::tri::UpdateNormals<MyMesh>::PerFace(mesh);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mesh);

		tm.Update();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(.7,.7,1.7,   0,0,0,   0,1,0);

		GLfloat lp[] = {.1,1,.2,0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		glRotatef(now,0,1,0);
		glScalef(1/meshSize,1/meshSize,1/meshSize);

		glTranslatef(-c.X(),-c.Y(),-c.Z());

		glColor3f(1,1,1);
		switch (DRAW_MODE){
		case 0: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMNone, vcg::GLW::TMNone> ();	break;
		case 1: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> (); break;
		case 2: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone> (); break;
		case 3: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> (); break;
		case 4: tm.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> (); break;
		}
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		const double SPF = 50/1000.0;
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
