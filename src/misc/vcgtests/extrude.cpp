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
class MyFace: public Face< MyUsedTypes, face::Color4b, face::FFAdj, face::VFAdj, face::Mark, face::VertexRef, face::Normal3f, face::BitFlags, face::InfoOcf> {};
class MyMesh: public vcg::tri::TriMesh< std::vector< MyVertex>, std::vector< MyFace > > {};

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

// void extrude(MyMesh* m, MyMesh::VertexType* v, double amount);

// Note, pass a reference to the pointer, so we can modify what its pointing to...
typedef std::vector<MyMesh::VertexPointer*> VPUpdateList;
void extrude(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, int width, Point3f direction, double length, double expand = 0);

void growTentacle(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul);
void growSucker(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, double sz);

int main(int argc, char *argv[])
{
	setupGL();
	std::srand(std::time(NULL));

	MyMesh mesh;
	vcg::tri::Sphere<MyMesh>(mesh);

	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);

	/*
	for(int i=0;i<4;i++)
		Refine(mesh,MidPoint<MyMesh>(&mesh));

	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
	*/

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

	vcg::tri::UpdateColor<MyMesh>::FaceConstant(mesh,Color4b::White);

	//MyMesh::VertexType* v = &mesh.vert[(int)(_random()*(mesh.vert.size()-1))];
	//extrude(&mesh, v, 1);

	std::set<MyMesh::VertexPointer> seedPoints;
	for(int i=0;i<10;i++){
		seedPoints.insert(&mesh.vert[(int)(_random()*(mesh.vert.size()-1))]);
	}
	std::list<MyMesh::VertexPointer> sp(seedPoints.begin(),seedPoints.end());
	VPUpdateList pointersToWatch;
	BOOST_FOREACH(MyMesh::VertexPointer& v, sp){
		pointersToWatch.push_back(&v);
	}

	BOOST_FOREACH(MyMesh::VertexPointer& v, sp){
		growTentacle(&mesh,v,pointersToWatch);
		// growSucker(&mesh,v,pointersToWatch);

		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
		vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);
	}

	//vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mesh);
	//vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mesh);

	GlTrimesh<MyMesh> tm;
	tm.m = &mesh;
	tm.Update();

	float rotate = 0;
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::Point3<float> c = mesh.bbox.Center();
	float meshSize = mesh.bbox.DimY();

	int running = GL_TRUE;

	while(running){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double now = glfwGetTime();

		/*
		MyMesh::VertexIterator it = mesh.vert.begin();

		for (;it!=mesh.vert.end();++it){
			double p[3] = {it->P()[0],it->P()[1],it->P()[2]};
			double val = math::Perlin::Noise(p[0],p[1],p[2]+now);
			for (int i=0;i<3;i++)
				it->P()[i] += val*_random(0,.01);
		}
		*/

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

void growTentacle(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul){
	// do a bunch of extrudes on the vertex
	// Perturb the the normal a bit
	Point3f n = v->N();
	int suckerCounter = 3;

	const int numSegments = 10;

	const double startSegLength = .3;
	const double segLengthDiff = -(startSegLength-.05) * 1./numSegments;
	double segLength = startSegLength;
	const int numSubSegments = 3;

	// const double segWidthDifference = -.005;
	double segWidth = -0.05;

	for(int k=0;k<numSegments;k++){
		Point3f rV = Point3f(_random(-1,1),_random(-1,1),_random(-1,1));
		rV /= vcg::Norm(rV);
		n += rV*.3;
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
	Point3f n = v->N();

	extrude(m, v, vpul, 2, n, sz, 0);
	extrude(m, v, vpul, 2, n, .01*sz, -.5);
	extrude(m, v, vpul, 2, n, -.8*sz, 0);

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(*m);
}

/**
 * Checks if loop is a connected closed edge loop.
 *
 * @param loop
 * @return
 */
bool isEdgeLoop(std::vector<MyMesh::VertexPointer>& loop){
	//std::cout << "isEdgeLoop: loop.size = " << loop.size() << "\n";

	if (loop.size()<3) return false;
	MyMesh::VertexPointer p = loop[loop.size()-1];
	BOOST_FOREACH(MyMesh::VertexPointer np, loop){
		// p should be connected to np
		bool isPConnectedToNP = false;
		vcg::face::VFIterator<MyMesh::FaceType> vfi(p);
		while(!vfi.End()){
			if (vfi.f->V(0)==np || vfi.f->V(1)==np || vfi.f->V(2)==np)
			{
				isPConnectedToNP = true;
				break;
			}
			++vfi;
		}

		if (!isPConnectedToNP){
			return false;
		}
		p = np;
	}
	return true;
}

void extrude(MyMesh* m, MyMesh::VertexType*& v, VPUpdateList& vpul, int width, Point3f direction, double length, double expand){
	typedef MyMesh::VertexType Vertex;
	typedef MyMesh::VertexPointer VertexPointer;
	typedef MyMesh::FaceType Face;
	typedef MyMesh::FacePointer FacePointer;

	Point3f center = v->P();

	assert (width >= 1);

	// select the one-ring of faces
	vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
	vcg::tri::Nring<MyMesh> ring(v,m);
	ring.expand(width); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
	std::vector<FacePointer> internalFaces = ring.allF;
	std::vector<FacePointer> lastF = ring.lastF;
	// std::vector<VertexPointer> edgeLoop = ring.lastV;

	// extract the ordered edgeLoop from the Nring
	std::vector<VertexPointer> lastV = ring.lastV;
	std::vector<VertexPointer> edgeLoop;
	Vertex* v0 = lastV[0];
	edgeLoop.push_back(v0);
	Vertex* oldV = v0;
	std::vector<VertexPointer>::iterator lastVEnd = std::remove(lastV.begin(), lastV.end(), v0);;
	while (lastVEnd!=lastV.begin()){

		/*
		std::cout << "lastV: ";
		std::vector<VertexPointer>::iterator it = lastV.begin();
		for(;it!=lastVEnd;++it){
			std::cout << *it << " ";
		}
		std::cout << "\n";
		*/

		// find neighbour of v0 in internal Faces
		bool foundNextItem = false;
		BOOST_FOREACH(FacePointer fp, lastF){
			for(int k=0;k<3;k++){
				if (fp->V(k)==oldV){
					Vertex* nextV = fp->V((k+1)%3);
					//std::cout << "nextV: " << nextV << "\n";
					// if nextV is in lastV then it is the next item in the edge list
					std::vector<VertexPointer>::iterator it = std::find(lastV.begin(),lastVEnd,nextV);
					if (it!=lastVEnd){
						//std::cout << "found\n";
						// found the next vertex
						edgeLoop.push_back(nextV);
						oldV = nextV;
						foundNextItem = true;
						lastVEnd = std::remove(lastV.begin(), lastVEnd, nextV);
						break;
					}
					else {
						//std::cout << "NOT found\n";
					}
				}
			} // exit break

			if (foundNextItem) break;
		}

		if (foundNextItem==false) {
			// the
			error("extrude: Didn't find the next vertex in the edge loop");
			break;
		}
	}
	// implicitly, the last element should be connected to the first


	// clear ring here, as its internal refs will get screwed up when
	// we add verts later on..
	ring.clear();

	if (!isEdgeLoop(edgeLoop)){
		error("extrude: Edge loop is either not connected or not closed.");
	}

	// Duplicate vertices of edge ring
	// (v' corresponding to v in the edge ring)
	// insideedgeLoop[i] = edgeLoop[i]'
	std::vector<Vertex*> insideEdgeLoop;
	int newVertsStartIndex = m->vert.size();

	// Add new vertices, but note that we need to
	// update the pointers to v and each edgeLoop vertex
	std::vector<VertexPointer*> vtxPtrs;
	BOOST_FOREACH(VertexPointer& ev, edgeLoop){
		vtxPtrs.push_back(&ev);
	}
	vtxPtrs.push_back(&v);
	BOOST_FOREACH(VertexPointer*& ev, vpul){
		vtxPtrs.push_back(ev);
	}

	Allocator<MyMesh>::AddVertices(*m,edgeLoop.size(),vtxPtrs);

	int di = 0;
	BOOST_FOREACH(Vertex* ev, edgeLoop){
		Vertex* nv = &m->vert[newVertsStartIndex + di];
		insideEdgeLoop.push_back(nv);
		nv->P() = ev->P(); //  + direction*amount; // moved later..
		di++;
	}

	// For all v in edgeLoop
	// for all f attached to v
	// If f is an inside face, then reattach it to v'

	// The reattachments we need to perform
	// (f,i,v) set f->V(i)=v
	std::list<boost::tuple<Face*,int,Vertex*> > faceAdjustments;

	for(int i=0;i<edgeLoop.size();i++){
		Vertex* ve = edgeLoop[i];
		Vertex* vm1 = edgeLoop[(i-1+edgeLoop.size())%edgeLoop.size()];
		Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
		Vertex* vd = insideEdgeLoop[i];

		// How to find inside faces?
		// 1. first find the face that contains ve,vp1
		Face* startFace = NULL;
		int startIndex = 0; // The index of ve in startFace

		//std::cerr << "ve: " << ve << "\n";

		vcg::face::VFIterator<Face> vfi(ve); //initialize the iterator to the first face
		for(;!vfi.End();++vfi)
		{
			Face* f = vfi.F();
			// std::cerr << "f: " << f << "\n";

			int k=0;
			for(;k<3;k++){
				if (f->V(k)==ve && f->V((k+1)%3)==vp1){
					startFace = f;
					startIndex = k;
					break;
				}
			}
			if (k==3) continue;
			else {
				// we have found the start face
				break;
			}
		}

		if (startFace==NULL){
			// An error has occurred
			// The edge ring is not connected properly
			// TODO: Throw an informative error message
			error("extrude: edge loop probably not fully connected or closed");
		}

		// 2. Next, iterate from this face, until we find the endFace
		// the endFace contains vm1,ve in order
		int cycle = 0; // track how many iterations we do and stop if too many
		vcg::face::Pos<Face> p(startFace,(startIndex+2)%3,ve);
		do {
			// std::cerr << ".\n";
			// Find index of ve in p.f
			int index = 0;
			for(;index<3;index++)
			{ if (p.f->V(index)==ve) break; }
			assert(index!=3);
			faceAdjustments.push_back(boost::make_tuple(p.f,index,vd));

			// iterate to next face, stopping when we cross the edgering
			// i.e., edge p.E() joins ve and vm1
			// as p.E() always starts at ve, we just need to check vm1
			p.FlipF();
			if (p.f->V((p.E()+1)%3)==vm1) break;
			// else
			p.FlipE();

			cycle++;
		} while (cycle < 100); // shouldnt have more than 100 neighbour faces!
		if (cycle==100){
			error("extrude: Caught in an infinite loop around a vertex! Check your edge-ring or topology correctness.");
		}
	}

	typedef boost::tuple<Face*,int,Vertex*> FivTuple;
	BOOST_FOREACH(FivTuple& a, faceAdjustments){
		Face* f = a.get<0>();
		int i = a.get<1>();
		Vertex* v = a.get<2>();
		f->V(i) = v;
	}

	// Finally, for all pairs of edges, create two new triangles
	// between them
	int newFacesStartIndex = m->face.size();

	std::vector<FacePointer*> facePointersToUpdate;
	BOOST_FOREACH(FacePointer& f, internalFaces){
		facePointersToUpdate.push_back(&f);
	}
	Allocator<MyMesh>::AddFaces(*m,edgeLoop.size()*2,facePointersToUpdate);
	for(int i=0;i<edgeLoop.size();i++){
		Vertex* ve = edgeLoop[i];
		Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
		Vertex* vd = insideEdgeLoop[i];
		Vertex* vdp1 = insideEdgeLoop[(i+1)%insideEdgeLoop.size()];

		Face* f1 = &m->face[newFacesStartIndex+2*i];
		Face* f2 = &m->face[newFacesStartIndex+2*i+1];
		f1->V(0) = ve;
		f1->V(1) = vp1;
		f1->V(2) = vd;
		f2->V(0) = vd;
		f2->V(1) = vp1;
		f2->V(2) = vdp1;
	}

	// finally move all the internal faces
	// NB: make sure each vertex is only moved once
	std::set<VertexPointer> internalVerts;
	BOOST_FOREACH(FacePointer f, internalFaces){
		internalVerts.insert(f->V(0));
		internalVerts.insert(f->V(1));
		internalVerts.insert(f->V(2));
		f->C() = Color4b::Red;
	}

	BOOST_FOREACH(VertexPointer p, internalVerts){
		Point3f dP = (center-p->P())*-expand;
		p->P() += dP;
		p->P() += direction*length;
	}

	// Do a big ass topo update
	// TODO: can efficiencize this by manually fixing ptrs etc above
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(*m);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(*m);
}
















