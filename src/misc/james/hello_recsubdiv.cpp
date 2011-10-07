/**
 * Hello spline!
 */


#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/functions.h"
#include "fgv/trackball.h"

#include "fg/gc/interpolator.h"
#include "fg/gc/bezinterp.h"
#include "fg/gc/recsubdiv.h"

#include "fg/glrenderer.h"

// opengl viz, hack
#include "fg/meshimpl.h"
#include <wrap/gl/trimesh.h>
using vcg::GlTrimesh;

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

/*
void GLFWCALL keyCallback(int key, int action)
{
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		DRAW_MODE = (DRAW_MODE + 1)%NUM_DRAW_MODES;
	}
}
*/

void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

void mouseMoved(int x, int y);
void mouseButtoned(int button, int state);

using namespace fg;

int main(int argc, char *argv[])
{
	setupWindowAndGL();

	// ** Create new spline here
	const int numPoints = 2;
    //fg::spline::PBezInterp<Vec3> spline = spline::PBezInterp<Vec3>();
	std::vector<Vec3> arr;
	/*
    arr.push_back(Vec3(-4.6,0.,0.));
    arr.push_back(Vec3(1.2,-1.2,0.));
    arr.push_back(Vec3(1.2,1.2,0.));
    arr.push_back(Vec3(-4.6,-.6,0.));
	*/
    arr.push_back(Vec3(0.,0.,0.));
    arr.push_back(Vec3(.02,.5,0.));
    arr.push_back(Vec3(.08,-.5,0.));
    arr.push_back(Vec3(.1,0.,0.));
    fg::gc::RecSubDiv subdiv;
	subdiv.approximation_scale(90.);
	subdiv.angle_tolerance(0.05);
	subdiv.init( arr );
	std::vector<Vec3> subdivdata = subdiv.get_data();
	std::cout << subdivdata.size() << std::endl;

    arr[0] = arr[0] + Vec3( 0.2, 0., 0. );
    arr[1] = arr[1] + Vec3( 0.2, 0., 0. );
    arr[2] = arr[2] + Vec3( 0.2, 0., 0. );
    arr[3] = arr[3] + Vec3( 0.2, 0., 0. );
	fg::gc::BezInterp<Vec3> spline(arr);
	int size = subdivdata.size() - 1;
	std::vector<Vec3> incdata = spline.getApproxVector(size);
	std::cout << incdata.size() << std::endl;



	// Run as fast as I can
	bool running = true;

	double time = glfwGetTime();
	double dt = 0.01;

	while(running){
		// Recompute delta t
		double now = glfwGetTime();
		dt = now - time;
		time = now;

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

		// Draw stuff here
		glLineWidth( 2. );
		glColor3f(1,1,1);
		glBegin(GL_LINES);
		for(int i=0;i<subdivdata.size() - 1;i++){
			// draw a line from arr[i] to arr[i+1]
			glVertex3d(subdivdata[i].getX(),subdivdata[i].getY(),subdivdata[i].getZ());
			glVertex3d(subdivdata[i+1].getX(),subdivdata[i+1].getY(),subdivdata[i+1].getZ());
		}
		glEnd();
		glColor3f(1,0,0);
		glBegin(GL_LINES);
		for(int i=0;i<incdata.size() - 1;i++){
			// draw a line from arr[i] to arr[i+1]
			glVertex3d(incdata[i].getX(),incdata[i].getY(),incdata[i].getZ());
			glVertex3d(incdata[i+1].getX(),incdata[i+1].getY(),incdata[i+1].getZ());
		}
		glEnd();


		glPopMatrix();

		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

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

	glfwSetWindowTitle("hello spline!");
	//glfwSetKeyCallback(keyCallback);
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
	else if (button==GLFW_MOUSE_BUTTON_RIGHT){
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
