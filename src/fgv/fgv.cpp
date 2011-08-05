/**
 * A simple fg viewer...
 *
 * @author ben
 */

// Shaders arent working, gah
// #define USESHADERS

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
using namespace boost::filesystem;

#include "AntTweakBar.h"
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"
#include "fg/util.h"

#include "fgv/shader.h"
#include "fgv/trackball.h"

const double FPS = 60;
const double SPF = 1/FPS;
int gWidth = 800;
int gHeight = 600;

float gZoom = 0;
float gRotationQuat[4] = {0,0,0,1};
float gRotationMatrix[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
float gCameraTranslation[3] = {0,0,0};

struct MouseState {
	//bool leftButtonDown;
	//bool middleButtonDown;
	int lastX;
	int lastY;

	bool isRotating;
	bool isTranslating;
	bool isZooming;
} gMouseState = {0,0,false,false,false};

struct ViewMode {
	bool origin;
	bool ground;
	bool showNodeAxes; // show node axes

	enum MeshMode { MM_SMOOTH, MM_FLAT, MM_WIRE, MM_POINTS };
	MeshMode meshMode;

} gViewMode = {true,true,false,ViewMode::MM_SMOOTH};

void GLFWCALL keyCallback(int key, int action)
{
	if (not TwEventKeyGLFW(key,action)){
		// custom stuff...
	}
}

void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

void drawOrigin();
void drawGroundPlane();
void mouseMoved(int x, int y);
void mouseButtoned(int button, int state);

#ifdef USESHADERS
void loadShaders(); // loads the shader code into the buffers below

const char* shaderPTVert[1];
const char* shaderPTFrag[1];

const char* shaderPhongVert[1];
const char* shaderPhongFrag[1];
#endif

int main(int argc, char *argv[])
{
	if (argc!=2){
		std::cout << "Usage: " << argv[0] << " <script>\n";
		std::cout << "Example <script> is \"tests/basic5\" (note no .lua suffix needed\n";
		return 1;
	}
	else {
		std::cout << "fgv: fg viewer (c) 2011\n";
		std::cout << "Left Mouse: Rotate, Middle/Right Mouse: Zoom, Left Mouse + Ctrl/Shift/Alt: Move\n";
	}

	setupWindowAndGL();

#ifdef USESHADERS
	loadShaders();
	std::cout << "Compiling phong shader\n";
	static shader_prog phongShader(shaderPhongVert, shaderPhongFrag);
	std::cout << "Compiling passthru shader\n";
	static shader_prog passThruShader(shaderPTVert, shaderPTFrag);
	passThruShader();
#endif

	// Create a new universe
	fg::Universe u = fg::Universe();
	u.addScriptDirectory("../scripts/?.lua");
	u.loadScript(argv[1]);

	bool running = true;

	double before = glfwGetTime();
	// double dt = SPF;
	double now = glfwGetTime();

	// Run as fast as I can
	while(running){
		// compute how long this frame takes to update and draw
		before = glfwGetTime();

		// Update the universe
		u.update(SPF);

		// Draw all the meshes in the universe
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1,1,1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(2,2,2,   0,0,0,   0,1,0);

		GLfloat lp[] = {.1, 1, .1, 0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		glPushMatrix();
		glMultMatrixf((GLfloat*) gRotationMatrix);

		glTranslatef(gCameraTranslation[0],
				gCameraTranslation[1],
				gCameraTranslation[2]);

		float z = std::exp(-gZoom);
		glScalef(z,z,z);

		if (gViewMode.origin) drawOrigin();
		if (gViewMode.ground) drawGroundPlane();

		foreach(shared_ptr<fg::MeshNode> m, u.meshNodes()){
			m->mesh()->sync(); // make sure normals are okay

			fg::GLRenderer::RenderMeshMode rmm;
			switch (gViewMode.meshMode){
				case ViewMode::MM_SMOOTH: rmm = fg::GLRenderer::RENDER_SMOOTH; break;
				case ViewMode::MM_FLAT: rmm = fg::GLRenderer::RENDER_FLAT; break;
				case ViewMode::MM_WIRE: rmm = fg::GLRenderer::RENDER_WIRE; break;
				case ViewMode::MM_POINTS: rmm = fg::GLRenderer::RENDER_VERTICES; break;
			}
			fg::GLRenderer::renderMeshNode(m,rmm); // fg::GLRenderer::RenderMeshMode(DRAW_MODE));
		}

		if (gViewMode.showNodeAxes){
			foreach(shared_ptr<fg::Node> n, u.nodes()){
				glPushMatrix();
				fg::Mat4 t = n->getCompoundTransform().transpose();
				glMultMatrixd(t.V());
				fg::GLRenderer::renderAxes();
				glPopMatrix();
			}
		}
		glPopMatrix();

		TwDraw();
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		// Sleep so we don't exceed FPS
		now = glfwGetTime();
		double spareTime = SPF - (now - before);
		if (spareTime > 0) glfwSleep(spareTime);
	}

	TwTerminate();
	glfwTerminate();
	return EXIT_SUCCESS;
}

void setupWindowAndGL(){
	// Setup GL etc
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	GLFWvidmode mode;   // GLFW video mode
	// Create a window
	glfwGetDesktopMode(&mode);

	if (!glfwOpenWindow(gWidth,gHeight, mode.RedBits, mode.GreenBits, mode.BlueBits, 0,0,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwEnable(GLFW_MOUSE_CURSOR);
	glfwEnable(GLFW_KEY_REPEAT);

	/*
	if (!glfwOpenWindow(gWidth,gHeight, 0,0,0,0,8,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	*/

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle("fg viewer");

	// Window is loaded, set up gui
	TwInit(TW_OPENGL, NULL);
	TwBar* mainBar = TwNewBar("O_o");
	TwAddVarRW(mainBar, "origin", TW_TYPE_BOOLCPP, &gViewMode.origin,
	               " group='View' help='Toggle origin.' ");
	TwAddVarRW(mainBar, "ground", TW_TYPE_BOOLCPP, &gViewMode.ground,
	               " group='View' help='Toggle ground.' ");
	TwAddVarRW(mainBar, "node axes", TW_TYPE_BOOLCPP, &gViewMode.showNodeAxes,
		               " group='View' help='Toggle node axes.' ");

	TwEnumVal mmEV[] = { { ViewMode::MM_SMOOTH, "Smooth"},
			{ ViewMode::MM_FLAT, "Flat"},
			{ ViewMode::MM_WIRE, "Wireframe"},
			{ ViewMode::MM_POINTS, "Points"}};

	TwType mmType = TwDefineEnum( "Mesh Mode", mmEV, 4 );
	TwAddVarRW(mainBar, "view mesh", mmType, &gViewMode.meshMode,
			" group='View' keyIncr=Tab keyDecr=SHIFT+Tab help='Change the mesh rendering mode.' ");

	// after GLFW initialization
	// directly redirect GLFW events to AntTweakBar
	glfwSetWindowSizeCallback(resizeWindow);
	//glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	//glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
	glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
	//glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
	glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);

	glfwSetKeyCallback(keyCallback);
	glfwSetMousePosCallback(mouseMoved);
	glfwSetMouseButtonCallback(mouseButtoned);

	glClearColor(0, 0, 0, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat amb[] = {0,0,0,1};
	GLfloat diff[] = {1,1,1,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diff);
}

void GLFWCALL resizeWindow(int _width, int _height){
	gWidth = _width;
	gHeight = _height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0*gWidth/gHeight, 0.1, 100);
	glViewport(0,0,gWidth,gHeight);
	glMatrixMode(GL_MODELVIEW);

	TwWindowSize(gWidth, gHeight);  // Send the new window size to AntTweakBar
}

void GLFWCALL mouseMoved(int x, int y){


	if (gMouseState.isRotating){
		float q[4];

		float invWidth = 1.f/gWidth;
		float invHeight = 1.f/gHeight;

		float dLastX = 2 * gMouseState.lastX * invWidth - 1;
		float dLastY = -2 * (gMouseState.lastY * invHeight) + 1;
		float dX = 2*x*invWidth - 1;
		float dY = -2*y*invHeight + 1;

		trackball(q,dLastX,dLastY,dX,dY);

		float oldQ[4] = {gRotationQuat[0],gRotationQuat[1],gRotationQuat[2],gRotationQuat[3]};
		add_quats(q,oldQ,gRotationQuat);

		build_rotmatrix(gRotationMatrix, gRotationQuat);

		gMouseState.lastX = x;
		gMouseState.lastY = y;
	}
	else if (gMouseState.isZooming) {
		double dy = -1.0*(gMouseState.lastY - y)/gHeight;
		gZoom += dy*5;
		gMouseState.lastY = y;
	}
	else if (gMouseState.isTranslating) {
		float invWidth = 1.f/gWidth;
		float invHeight = 1.f/gHeight;
		float dLastX = 2 * gMouseState.lastX * invWidth - 1;
		float dLastY = -2 * (gMouseState.lastY * invHeight) + 1;
		float dX = 2*x*invWidth - 1;
		float dY = -2*y*invHeight + 1;

		// update position
		// gCameraTranslation[0] += 5*(dX-dLastX);
		gCameraTranslation[1] += 5*(dY-dLastY);

		gMouseState.lastX = x;
		gMouseState.lastY = y;
	}

	// else
	if (not TwEventMousePosGLFW(x,y)){
		// ...
	}
}

void GLFWCALL mouseButtoned(int button, int state){
	if (not TwEventMouseButtonGLFW(button,state)){

		gMouseState.isTranslating = false;
		gMouseState.isRotating = false;
		gMouseState.isZooming = false;

		/*
		if (state == GLFW_RELEASE){
			gMouseState.isTranslating = false;
			gMouseState.isRotating = false;
			gMouseState.isZooming = false;
		}
		else */
		if (state==GLFW_PRESS) {
			glfwGetMousePos(&gMouseState.lastX,&gMouseState.lastY);

			gMouseState.isTranslating = false;
			gMouseState.isRotating = false;
			gMouseState.isZooming = false;

			if (button==GLFW_MOUSE_BUTTON_LEFT and
					(glfwGetKey(GLFW_KEY_LCTRL) or
					 glfwGetKey(GLFW_KEY_LSHIFT) or
					 glfwGetKey(GLFW_KEY_LALT))){
				// Translation
				gMouseState.isTranslating = true;

			}
			else if (button == GLFW_MOUSE_BUTTON_LEFT){
				// Rotation
				gMouseState.isRotating = true;
			}
			else if (button==GLFW_MOUSE_BUTTON_MIDDLE || button==GLFW_MOUSE_BUTTON_RIGHT){
				// Zoom
				gMouseState.isZooming = true;
			}
		}
	}
}

void drawOrigin(){
	glPushAttrib(GL_LIGHTING_BIT);
	glPushAttrib(GL_LINE_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(2);
	fg::GLRenderer::renderAxes();
	glPopAttrib();
	glPopAttrib();
}

void drawGroundPlane(){
	glPushAttrib(GL_LIGHTING_BIT);
	glPushAttrib(GL_LINE_BIT);
	glPushAttrib(GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(1);
	glColor3f(.5,.5,.5);
	const int WIDTH = 5;
#define F(fx,fz,tx,tz) {glVertex3f(fx,0,fz); glVertex3f(tx,0,tz);}
	glBegin(GL_LINES);
		for(int x=-WIDTH;x<=WIDTH;x++)
			F(x,-WIDTH,x,WIDTH);
		for(int z=-WIDTH;z<=WIDTH;z++)
			F(-WIDTH,z,WIDTH,z);
	glEnd();
#undef F
	glPopAttrib();
	glPopAttrib();
	glPopAttrib();
}

#ifdef USESHADERS

void _loadShader(std::string vertfile, std::string fragfile, const char* shaderVert[1], const char* shaderFrag[1]){
	std::ifstream pvfile(vertfile.c_str(),std::ios::in|std::ios::ate);
	if (!pvfile.is_open()){
		std::cerr << "Can't open shader: " << vertfile << "\n";
		exit(1);
	}
	std::ifstream::pos_type size;
	char* memblock = new char[1024*1024];
	size = pvfile.tellg();
	pvfile.seekg (0, std::ios::beg);
	pvfile.read (memblock, size);
	pvfile.close();
	shaderVert[0] = memblock;

	std::ifstream pffile(fragfile.c_str(),std::ios::in|std::ios::ate);
	if (!pffile.is_open()){
		std::cerr << "Can't open shader: " << fragfile << "\n";
		exit(1);
	}
	char* memblock2 = new char[1024*1024];
	size = pffile.tellg();
	pffile.seekg (0, std::ios::beg);
	pffile.read (memblock2, size);
	pffile.close();
	shaderFrag[0] = memblock2;
	pffile.close();
}

void loadShaders(){ // loads the shader code into the buffers below
	std::string prefix = "../assets/shaders/";

	_loadShader(prefix+"passthru.vert",prefix+"passthru.frag",shaderPTVert,shaderPTFrag);
	_loadShader(prefix+"phong.vert",prefix+"phong.frag",shaderPhongVert,shaderPhongFrag);
}

#endif




















