/**
 * A simple fg viewer...
 *
 * @author ben
 * @date 06.07.2011
 */

#include "fg/fg.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"

#include "fgv/trackball.h"

#include <iostream>
#include <cmath>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "GL/glew.h"
#include "GL/glfw.h"

const double FPS = 60;
const double SPF = 1/FPS;
int gWidth = 800;
int gHeight = 600;

float gZoom = 0;
float gRotationQuat[4] = {0,0,0,1};
float gRotationMatrix[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
struct MouseState {
	bool leftButtonDown;
	bool middleButtonDown;
	int lastX;
	int lastY;
} gMouseState = {false,0,0};

int DRAW_MODE = 0; // start in flat
const int NUM_DRAW_MODES = 4;

void GLFWCALL keyCallback(int key, int action)
{
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		DRAW_MODE = (DRAW_MODE + 1)%NUM_DRAW_MODES;
	}
}

void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

void drawOrigin();
void drawGroundPlane();
void mouseMoved(int x, int y);
void mouseButtoned(int button, int state);

int main(int argc, char *argv[])
{
	if (argc!=2){
		std::cout << "Usage: " << argv[0] << " <script>\n";
		std::cout << "Example <script> is \"tests/basic5\" (note no .lua suffix needed\n";
		return 1;
	}
	else {
		std::cout << "fgv: fugu viewer (c) 2011\n";
		std::cout << "Left Mouse: Rotate, Middle/Right Mouse: Zoom\n";
	}

	setupWindowAndGL();

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
		float z = std::exp(-gZoom);
		glScalef(z,z,z);

		drawOrigin();
		drawGroundPlane();

		BOOST_FOREACH(boost::shared_ptr<fg::MeshNode> m, u.meshNodes()){
			m->mesh()->sync(); // make sure normals are okay
			fg::GLRenderer::renderMeshNode(m,fg::GLRenderer::RenderMeshMode(DRAW_MODE));
		}

		BOOST_FOREACH(boost::shared_ptr<fg::Node> n, u.nodes()){
			glPushMatrix();
			fg::Mat4 t = n->getCompoundTransform().transpose();
			glMultMatrixd(t.V());
			fg::GLRenderer::renderAxes();
			glPopMatrix();
		}

		glPopMatrix();

		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		// Sleep so we don't exceed FPS
		now = glfwGetTime();
		double spareTime = SPF - (now - before);
		if (spareTime > 0) glfwSleep(spareTime);
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

void setupWindowAndGL(){
	// Setup GL etc
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	if (!glfwOpenWindow(gWidth,gHeight, 0,0,0,0,8,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle("fugu viewer");
	glfwSetKeyCallback(keyCallback);
	glfwSetWindowSizeCallback(resizeWindow);
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
}

void GLFWCALL mouseMoved(int x, int y){
	if (gMouseState.leftButtonDown){
		// std::cout << "Dragging: " << x << "," << y << "\n";
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
	else if (gMouseState.middleButtonDown) {
		double dy = -1.0*(gMouseState.lastY - y)/gHeight;
		gZoom += dy*5;
		gMouseState.lastY = y;
	}
}

void GLFWCALL mouseButtoned(int button, int state){
	if (button == GLFW_MOUSE_BUTTON_LEFT){
		if (state==GLFW_PRESS){
			gMouseState.leftButtonDown = true;
			glfwGetMousePos(&gMouseState.lastX,&gMouseState.lastY);
		}
		else if (state==GLFW_RELEASE){
			gMouseState.leftButtonDown = false;
		}
	}
	else if (button==GLFW_MOUSE_BUTTON_MIDDLE || button==GLFW_MOUSE_BUTTON_RIGHT){
		if (state==GLFW_PRESS){
			gMouseState.middleButtonDown = true;
			glfwGetMousePos(&gMouseState.lastX,&gMouseState.lastY);
		}
		else if (state==GLFW_RELEASE){
			gMouseState.middleButtonDown = false;
		}
	}
	else {
		// avoid any mouse button trickery
		gMouseState.leftButtonDown = false;
		gMouseState.middleButtonDown = false;
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






















