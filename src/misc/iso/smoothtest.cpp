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
#include <GL/gl.h>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/math/perlin_noise.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>
//#include <vcg/simplex/vertex/with/afvn.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>

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

// for subdiv
/*
#include<vcg/simplex/face/component_ocf.h>
#include<vcg/simplex/face/topology.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/refine.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/bitquad_creation.h>
 */
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


int main(int argc, char *argv[])
{
	int running = GL_TRUE;
	int width = 800;
	int height = 800;

	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	if (!glfwOpenWindow(width,height, 0,0,0,0,8,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	MyMesh mc_mesh;
	vcg::tri::Icosahedron<MyMesh>(mc_mesh);

	// update infos
	vcg::tri::UpdateBounding<MyMesh>::Box(mc_mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(mc_mesh);
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(mc_mesh);

	Refine(mc_mesh,MidPointButterfly<MyMesh>());
	Refine(mc_mesh,MidPointButterfly<MyMesh>());
	Refine(mc_mesh,MidPointButterfly<MyMesh>());

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);


	GlTrimesh<MyMesh> tm;
	tm.m = &mc_mesh;
	tm.Update();

	glClearColor(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glLineWidth(2.0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat amb[] = {0,0,0,1};
	GLfloat diff[] = {1,1,1,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diff);

	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,50);

	GLfloat col[] = {1.f,0.f,1.f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);
	GLfloat white[] = {1.f,1.f,1.f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);

	float rotate = 0;
	vcg::Point3<float> c = mc_mesh.bbox.Center();
	float meshSize = mc_mesh.bbox.DimY();

	std::cout << "meshSize: " << meshSize << "\n";

	while(running){
		double time = glfwGetTime();

		// update
		// do something...

		// Smooth normals, shift verts, etc..
		/*
		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
		tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);
		tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mc_mesh,3);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
		*/

		/*
		// mc_mesh.face.EnableFFAdjacency();
		tri::UpdateTopology<MyMesh>::FaceFace(mc_mesh);
		tri::UpdateFlags<MyMesh>::FaceBorderFromFF(mc_mesh);
		tri::UpdateNormals<MyMesh>::PerVertexNormalized(mc_mesh);

		tri::BitQuadCreation<MyMesh>::MakePureByCatmullClark(mc_mesh);
		tri::UpdateNormals<MyMesh>::PerBitQuadFaceNormalized(mc_mesh);
		 */

		// update bounding box
		// vcg::tri::UpdateBounding<MyMesh>::Box(mc_mesh);

		// vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
		//vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);
		//vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mc_mesh);

		/*
		vcg::tri::UpdateNormals<MyMesh>::PerFace(mc_mesh);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mc_mesh);
		 */



		tm.m = &mc_mesh;
		tm.Update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(40, 1.0*width/height, 0.1, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(.7,.7,1.7,   0,0,0,   0,1,0);

		glColor3f(1,1,1);

		GLfloat lp[] = {0,1,1,0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		glRotatef(10*time,0,1,0);
		// glScalef(.15,.15,.15);
		glScalef(1/meshSize,1/meshSize,1/meshSize);

		glTranslatef(-c.X(),-c.Y(),-c.Z());

		tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMNone, vcg::GLW::TMNone> ();
		// tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
		// tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone> ();
		// tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
		// tm.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();

		glfwSwapBuffers();

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
};


void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
        free(infoLog);
    }
}

