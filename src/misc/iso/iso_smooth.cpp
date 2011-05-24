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

void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

void setupShaders(){
	const char* vscode[] = {
			"varying vec4 diffuse,ambient;\
			varying vec3 normal,lightDir,halfVector;\
\
			void main()\
			{\
				/* first transform the normal into eye space and\
				normalize the result */\
				normal = normalize(gl_NormalMatrix * gl_Normal);\
				/* now normalize the light's direction. Note that\
\
				according to the OpenGL specification, the light\
				is stored in eye space. Also since we're talking about\
				a directional light, the position field is actually direction */\
				lightDir = normalize(vec3(gl_LightSource[0].position));\
				/* Normalize the halfVector to pass it to the fragment shader */\
\
				halfVector = normalize(gl_LightSource[0].halfVector.xyz);\
				/* Compute the diffuse, ambient and globalAmbient terms */\
\
				diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;\
				ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;\
				ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;\
				gl_Position = ftransform();}"};


	const GLint vslen[] = {strlen(vscode[0])};

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vscode, vslen);
	glCompileShader(vs);
	printShaderInfoLog(vs);

	const char* fscode[] = {
"varying vec4 diffuse,ambient;\
varying vec3 normal,lightDir,halfVector;\
\
void main()\
{\
	vec3 n,halfV;\
	float NdotL,NdotHV;\
	/* The ambient term will always be present */\
\
	vec4 color = ambient;\
	/* a fragment shader can't write a varying variable, hence we need\
\
	a new variable to store the normalized interpolated normal */\
	n = normalize(normal);\
	/* compute the dot product between normal and ldir */\
\
	NdotL = max(dot(n,lightDir),0.0);\
	if (NdotL > 0.0) {\
			color += diffuse * NdotL;\
			halfV = normalize(halfVector);\
			NdotHV = max(dot(n,halfV),0.0);\
			color += gl_FrontMaterial.specular *\
					gl_LightSource[0].specular *\
					pow(NdotHV, gl_FrontMaterial.shininess);\
		}\
\
		gl_FragColor = color;\
\
}"};
	const GLint fslen[] = {strlen(fscode[0])};

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fscode, fslen);
	glCompileShader(fs);
	printShaderInfoLog(fs);

	GLuint pr = glCreateProgram();
	glAttachShader(pr,vs);
	glAttachShader(pr,fs);
	glLinkProgram(pr);
	glUseProgram(pr);
	printProgramInfoLog(pr);
}

void error(const char* msg){
	std::cerr << msg;
	exit(-1);
}

void error(const char* msg, const char* args){
	fprintf(stderr, msg, args);
	exit(-1);
}

double noise(double x, double y, double z){
	return math::Perlin::Noise(x,y,z);
}

extern "C" int init(lua_State* L){
	using namespace luabind;
	open(L);
	module(L)[
		def("noise", &noise)
	];
	return 0;
}

double callF(lua_State* L, double x, double y, double z, double t){
	lua_getglobal(L, "f");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);
	lua_pushnumber(L, t);
	if (lua_pcall(L, 4, 1, 0)!=0){
		error("Error running function \"f\": %s",lua_tostring(L, -1));
	}
	// else
	if (!lua_isnumber(L,-1)){
		error("function \"f\" didn't return a value!");
	}
	// else
	double result = lua_tonumber(L,-1);
	lua_pop(L, 1); // pop z, NB: function and args have been removed by pcall
	return result;
}

void callG(lua_State* L, double p[], double t, double dp[]){
	lua_getglobal(L, "g");
	lua_pushnumber(L, p[0]);
	lua_pushnumber(L, p[1]);
	lua_pushnumber(L, p[2]);
	lua_pushnumber(L, t);
	if (lua_pcall(L, 4, 1, 0)!=0){
		error("Error running function \"g\": %s",lua_tostring(L, -1));
	}
	// else
	if (!lua_istable(L,-1)){
		error("function \"g\" didn't return an array!");
	}
	// else
	for(int i=0;i<3;i++){
		lua_pushnumber(L, i+1);
		lua_gettable(L, -2);  /* get array[i+1] */
		if (!lua_isnumber(L, -1)){
			std::ostringstream oss;
			oss << "array doesnt contain a number!";
			error(oss.str().c_str());
		}
		dp[i] = lua_tonumber(L, -1);
		lua_pop(L,1);
	}
	lua_pop(L,1); // pop table
}

const char* scriptFileName = NULL;
lua_State *L;
bool loadingScript = false;

int DRAW_MODE = 0;
const int NUM_DRAW_MODES = 5;

void GLFWCALL keyCallback(int key, int action)
{
	/*
  if (key == GLFW_KEY_ESC && action == GLFW_PRESS)
    quit_the_program = 1;
  else
	  */
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		DRAW_MODE = (DRAW_MODE + 1)%NUM_DRAW_MODES;
	}

	if (!loadingScript){
		if (key == GLFW_KEY_SPACE and action == GLFW_PRESS){
			loadingScript = true;
			std::cout << "Reloading script (t=" << glfwGetTime() << "s)\n";
			if (luaL_dofile (L, scriptFileName)){
				error(lua_tostring(L, -1));
			}
			loadingScript = false;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc!=2){
		std::cerr << "Usage: " << argv[0] << " <script.lua>\n";
		exit(0);
	}
	// else
	scriptFileName = argv[1];

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

	glfwSetKeyCallback(keyCallback);

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	L = lua_open();   /* opens Lua */
	luaL_openlibs(L);             /* opens the libraries */
	init(L);

	if (luaL_dofile (L, scriptFileName)){
		error(lua_tostring(L, -1));
	}

	setupShaders();

	MyVolume	volume;

	typedef vcg::tri::TrivialWalker<MyMesh,MyVolume> MyWalker;
	typedef vcg::tri::MarchingCubes<MyMesh,MyWalker> MyMarchingCubes;
	MyWalker walker;

	// Simple initialization of the volume with some cool perlin noise
	int VOXELSIZE = 14;

	volume.Init(Point3i(VOXELSIZE,VOXELSIZE,VOXELSIZE));

	for(int i=0;i<VOXELSIZE;i++)
		for(int j=0;j<VOXELSIZE;j++)
			for(int k=0;k<VOXELSIZE;k++)
			{
				// volume.Val(i,j,k)=(j-32)*(j-32)+(k-32)*(k-32)  + i*10*(float)math::Perlin::Noise(i*.2,j*.2,k*.2);
				// volume.Val(i,j,k)=(j-VOXELSIZE/2)*(j-VOXELSIZE/2)+(k-VOXELSIZE/2)*(k-VOXELSIZE/2) + i*10*(float)math::Perlin::Noise(i*.1,j*.4,k*.4);
				// volume.Val(i,j,k) = (i>VOXELSIZE/2)?-1:1;
				double di = 2.0*i/VOXELSIZE - 1;
				double dj = 2.0*j/VOXELSIZE - 1;
				double dk = 2.0*k/VOXELSIZE - 1;
				volume.Val(i,j,k) = di*di + dj*dj + dk*dk - .5;
			}

	// MARCHING CUBES
	MyMesh mc_mesh;
	MyMarchingCubes	mc(mc_mesh, walker);
	walker.BuildMesh<MyMarchingCubes>(mc_mesh, volume, mc, 0); //20*20);

	// update bounding box
	vcg::tri::UpdateBounding<MyMesh>::Box(mc_mesh);
	// update Normals

	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);

	//vcg::tri::UpdateNormals<MyMesh>::PerVertexPerFace(mc_mesh);

	GlTrimesh<MyMesh> tm;
	tm.m = &mc_mesh;
	tm.Update();

	// vcg::tri::io::ExporterPLY<MyMesh>::Save( mc_mesh, "marching_cubes.ply");
	// printf("OK!\n");
	glClearColor(.6, 0, .6, 1);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat amb[] = {0,0,0,1};
	GLfloat diff[] = {1,1,1,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diff);

	GLfloat global_ambient[] = { 0.3f, 0.0f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);

	GLfloat col[] = {1.f,0.f,1.f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);
	GLfloat white[] = {1.f,1.f,1.f,0.f};
	GLfloat spec[] = {.5f,.5f,.5f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);

	float rotate = 0;
	vcg::Point3<float> c = mc_mesh.bbox.Center();
	float meshSize = mc_mesh.bbox.DimY();

	while(running){
		double time = glfwGetTime();

		if (!loadingScript){
			GLfloat col[] = {0,1,0,0.f};
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);

			// update
			for(int i=0;i<VOXELSIZE;i++){
				double di = 2.0*i/VOXELSIZE - 1;
				for(int j=0;j<VOXELSIZE;j++){
					double dj = 2.0*j/VOXELSIZE - 1;
					for(int k=0;k<VOXELSIZE;k++){
						double dk = 2.0*k/VOXELSIZE - 1;

						volume.Val(i,j,k) = (float) callF(L, di,dj,dk,time);
					}
				}
			}
			MyMarchingCubes	mc(mc_mesh, walker);
			// walker.BuildMesh<MyMarchingCubes>(mc_mesh, volume, mc, 20*20);
			walker.BuildMesh<MyMarchingCubes>(mc_mesh, volume, mc, 0);

			vcg::tri::UpdateTopology<MyMesh>::FaceFace(mc_mesh);
			vcg::tri::UpdateTopology<MyMesh>::VertexFace(mc_mesh);

			Refine(mc_mesh,MidPoint<MyMesh>(&mc_mesh));
			tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mc_mesh,2);

			// Refine(mc_mesh,MidPoint<MyMesh>(&mc_mesh));

			// Refine(mc_mesh,MidPointButterfly<MyMesh>());

			MyMesh::VertexIterator it = mc_mesh.vert.begin();
			for (;it!=mc_mesh.vert.end();++it){
				double dp[3];
				double p[3] = {it->P()[0],it->P()[1],it->P()[2]};
				callG(L,p,time,dp);
				it->P()[0] += dp[0];
				it->P()[1] += dp[1];
				it->P()[2] += dp[2];
			}

			// Refine(mc_mesh,MidPointButterfly<MyMesh>());
			tri::Smooth<MyMesh>::VertexCoordLaplacianHC(mc_mesh,2);

			//vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);
			//tri::UpdateNormals<MyMesh>::PerFaceNormalized(mc_mesh);
			//vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mc_mesh);

			vcg::tri::UpdateNormals<MyMesh>::PerFace(mc_mesh);
			vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mc_mesh);

			tm.m = &mc_mesh;
			tm.Update();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_PROJECTION);
			glColor3f(1,0,1);
			glLoadIdentity();
			gluPerspective(40, 1.0*width/height, 0.1, 100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(.7,.7,1.7,   0,0,0,   0,1,0);

			GLfloat lp[] = {.1,1,.2,0};
			glLightfv(GL_LIGHT0,GL_POSITION,lp);

			glRotatef(time,0,1,0);
			// glScalef(.15,.15,.15);
			glScalef(1/meshSize,1/meshSize,1/meshSize);

			glTranslatef(-c.X(),-c.Y(),-c.Z());

			switch (DRAW_MODE){
				case 0: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMNone, vcg::GLW::TMNone> ();
				break;
				case 1: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
				break;
				case 2: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone> ();
				break;
				case 3: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
				break;
				case 4: tm.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
				break;
			}
		}

		glfwSwapBuffers();

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	lua_close(L);

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

