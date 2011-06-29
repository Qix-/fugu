#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <sstream>
#include <list>
#include <set>
#include <algorithm>

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

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/math/perlin_noise.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/simplex/face/pos.h>

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

#include <vcg/complex/algorithms/refine.h>
#include <vcg/complex/algorithms/nring.h>

// for opengl
#include <wrap/gl/trimesh.h>

// profiling
#include <boost/timer.hpp>
#include <boost/progress.hpp>

// extrusion
#include "fg/meshimpl.h"
#include "fg/meshoperators_vcg.h"

using namespace std;
using namespace vcg;

//class MyVertex: public Vertex< MyUsedTypes, vertex::Coord3d, vertex::Normal3d, vertex::BitFlags, vertex::VFAdj, vertex::InfoOcf, vertex::Mark>{};
//class MyFace: public Face< MyUsedTypes, face::Color4b, face::FFAdj, face::VFAdj, face::Mark, face::VertexRef, face::Normal3d, face::BitFlags, face::InfoOcf> {};
//class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

typedef fg::VertexImpl MyVertex;
typedef fg::FaceImpl MyFace;
typedef fg::MeshImpl MyMesh;
typedef fg::Extrude::VPUpdateList VPUpdateList;

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
	return (1.0*rand())/RAND_MAX;
}

double _random(double low, double high){
	return _random()*(high-low) + low;
}

class ExtrudeC {
public:
	ExtrudeC(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, int width, Point3d direction, double length, double expand = 0, double duration = 0){
		mEndVerts = fg::Extrude::extrude(m,
					//static_cast<fg::Extrude<MyMesh>::Vertex*&>(v),
					//static_cast<fg::Extrude<MyMesh>::VPUpdateList&>(vpul),
					v,
					vpul,
					width,
					direction,
					0.001, /* length, */
					expand);
		mMesh = m;
		mLength = length - 0.001;
		mCurrentTime = 0;
		mStartPos = v->P() + direction*0.001;
		mDirection = direction;
		mEndTime = duration;
	}

	void update(double dt){
		if (mCurrentTime > mEndTime) return; // TODO: Flag finish, remove module

		mCurrentTime += dt;
		// update position of vertices
		BOOST_FOREACH(fg::Extrude::VertexPointer vp, mEndVerts){
			vp->P() += mDirection*(dt*mLength/mEndTime);
		}

		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(*mMesh);
		vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(*mMesh);
	}

private:
	double mLength;
	double mCurrentTime;
	Point3d mStartPos;
	Point3d mDirection;
	double mEndTime;
	MyMesh* mMesh;
	std::set<fg::Extrude::VertexPointer> mEndVerts;

};



// void extrude(MyMesh* m, MyMesh::VertexType* v, double amount);
void extrude(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, int width, Point3d direction, double length, double expand = 0);
void growTentacle(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul);
void growSucker(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, double sz);

void monster(MyMesh& m);

int main(int argc, char *argv[])
{
	setupGL();
	std::srand(std::time(NULL));

	MyMesh mesh;
	vcg::tri::Sphere<MyMesh>(mesh);

	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);
	vcg::tri::UpdateColor<MyMesh>::FaceConstant(mesh,Color4b::White);

	std::list<ExtrudeC*> modules;
	for(int i=0;i<10;i++){
		MyMesh::VertexType* v = &mesh.vert[(int)(_random()*(mesh.vert.size()-1))];
		VPUpdateList vpul; vpul.push_back(&v);

		float mix = .2;
		Point3d direction = v->N()*(1-mix) + Point3d(0,1,0)*mix;
		direction /= vcg::Norm(direction);

		ExtrudeC* exc = new ExtrudeC(&mesh, v, vpul, 1, direction, .3, .1, 10);
		modules.push_back(exc);
	}

	/*
	for (int i=0;i<10;i++){
		MyMesh::VertexType* v = &mesh.vert[(int)(_random()*(mesh.vert.size()-1))];
		VPUpdateList vpul; vpul.push_back(&v);

		float mix = .5;
		Point3d direction = v->N()*(1-mix) + Point3d(0,1,0)*mix;
		direction /= vcg::Norm(direction);
		extrude(&mesh, v, vpul, 1, direction, .1, .1);
	}

	monster(mesh);
	*/


	GlTrimesh<MyMesh> tm;
	tm.m = &mesh;
	tm.Update();

	float rotate = 0;
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::Point3d c = mesh.bbox.Center();
	float meshSize = mesh.bbox.DimY();

	int running = GL_TRUE;

	const double SPF = 50/1000.0;

	while(running){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double now = glfwGetTime();

		BOOST_FOREACH(ExtrudeC* m, modules){
			m->update(SPF);
		}

		tm.Update();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(.7,.7,1.7,   0,0,0,   0,1,0);

		GLfloat lp[] = {.1,-2,.2,0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);
		GLfloat lp2[] = {.2,3,.1,0};
		glLightfv(GL_LIGHT1,GL_POSITION,lp2);

		glRotatef(now*10,0,1,0);
		glScalef(1/meshSize,1/meshSize,1/meshSize);

		glTranslatef(-c.X(),-c.Y(),-c.Z());

		glColor3f(1,1,1);
		switch (DRAW_MODE){
		case 0: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();	break;
		case 1: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
		case 2: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> (); break;
		case 3: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
		case 4: tm.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
		}
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);


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
	glEnable(GL_LIGHT1);
	GLfloat amb[] = {0,0,0,1};
	GLfloat diff[] = {1,1,1,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diff);
	glLightfv(GL_LIGHT1,GL_AMBIENT,amb);
	GLfloat diff2[] = {.6,.6,.6,1};
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diff2);

	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);

	GLfloat col[] = {1.f,1.f,1.f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);
	GLfloat white[] = {1.f,1.f,1.f,0.f};
	GLfloat spec[] = {.5f,.5f,.5f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glColor3f(1,0,1);
	glLoadIdentity();
	gluPerspective(40, 1.0*width/height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);

}

void monster(MyMesh& mesh){
	std::set<MyMesh::VertexPointer> seedPoints;
	const int NUM_THINGS = 20;
	for(int i=0;i<NUM_THINGS;i++){
		seedPoints.insert(&mesh.vert[(int)(_random()*(mesh.vert.size()-1))]);
	}
	std::list<MyMesh::VertexPointer> sp(seedPoints.begin(),seedPoints.end());
	VPUpdateList pointersToWatch;
	BOOST_FOREACH(MyMesh::VertexPointer& v, sp){
		pointersToWatch.push_back(&v);
	}

	int i = 0;
	BOOST_FOREACH(MyMesh::VertexPointer& v, sp){
		if (_random() < .4)
			growSucker(&mesh,v,pointersToWatch,.2);
		else
			growTentacle(&mesh,v,pointersToWatch);

		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
		vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

		std::cout << i++ << " of " << NUM_THINGS << " things made ...\n";
	}

	//vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	//vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

	// Subdivide!
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	/*
	for(int i=0;i<1;i++)
		Refine(mesh,MidPoint<MyMesh>(&mesh));
		*/

	vcg::tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mesh,2);
	Refine(mesh,MidPointButterfly<MyMesh>());

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);
}

void growTentacle(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul){
	// do a bunch of extrudes on the vertex
	// Perturb the the normal a bit
	Point3d n = v->N();
	int suckerCounter = 3;

	const double attractor = .3;
	const int numSegments = 10;

	const double startSegLength = .3;
	const double segLengthDiff = -(startSegLength-.05) * 1./numSegments;
	double segLength = startSegLength;
	const int numSubSegments = 3;

	// const double segWidthDifference = -.005;
	double segWidth = -0.05;

	for(int k=0;k<numSegments;k++){
		// Point3d rV = Point3d(_random(-1,1),_random(-1,1),_random(-1,1));
		// rV /= vcg::Norm(rV);

		Point3d rV = Point3d(0,1,0);
		n = n*(1-attractor) + rV*attractor;
		n /= vcg::Norm(n);
		for(int i=0;i<numSubSegments;i++){
			extrude(m, v, vpul, 2, n, segLength/numSubSegments, segWidth);
		}

		segLength += segLengthDiff;
		// segWidth += segWidthDifference;

		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(*m);

		// after every 3 extrusions, grow a sucker on a random place at a
		// distance of 3 edges from the center

		suckerCounter--;
		if (suckerCounter<=0){
			suckerCounter = 3;

			vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
			vcg::tri::Nring<MyMesh> ring(v,m);
			ring.expand(4); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
			MyMesh::VertexType* sv = ring.lastV[(int)_random(0,ring.lastV.size()-1)];
			ring.clear();

			// to figure out the size, just compute the distance to a neighbouring vertex
			double sz = 1;
			for(int i=0;i<3;i++){
				if (sv->VFp()->V(i)!=sv){
					sz = vcg::Norm(sv->VFp()->V(i)->P() - sv->P());
					break;
				}
			}
			growSucker(m,sv,vpul,sz);
		}
	}
}

void growSucker(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, double sz){
	Point3d n = v->N();

	extrude(m, v, vpul, 2, n, sz, 0);
	extrude(m, v, vpul, 2, n, .01*sz, -.5);
	extrude(m, v, vpul, 2, n, -.8*sz, 0);

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(*m);
}


void extrude(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, int width, Point3d direction, double length, double expand){

	// Call the encapsulated version
	fg::Extrude::extrude(m,
			//static_cast<fg::Extrude<MyMesh>::Vertex*&>(v),
			//static_cast<fg::Extrude<MyMesh>::VPUpdateList&>(vpul),
			v,
			vpul,
			width,
			direction,
			length,
			expand);
	return;
}
















