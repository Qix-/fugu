/**
 * A simple fg viewer.
 *
 * @author ben
 * @date 27.06.2011
 */

#include "fg/fg.h"
#include "fg/functions.h"

#include "fgv/trackball.h"

#include <iostream>
#include <cmath>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "GL/glew.h"
#include "GL/glfw.h"

int gWidth = 800;
int gHeight = 600;

float gRotationQuat[4] = {0,0,0,1};
float gRotationMatrix[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
struct MouseState {
	bool leftButtonDown;
	int lastX;
	int lastY;
} gMouseState = {false,0,0};

void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

void mouseMoved(int x, int y);
void mouseButtoned(int button, int state);

int main(int argc, char *argv[])
{
	if (argc!=3){
		std::cout << "Usage: " << argv[0] << " <script-directory> <script>";
		return 1;
	}

	setupWindowAndGL();

	// Create a new universe
	fg::Universe u = fg::Universe();

	// Load a script
	boost::filesystem::path p(argv[1]);
	if (!boost::filesystem::is_directory(p))
		p.remove_filename();
	u.addScriptDirectory(p.string() + "/?.lua");
	u.loadScript(argv[2]);

	bool running = true;

	double time = glfwGetTime();
	double dt = 0.01;

	// Run as fast as I can
	while(running){
		// Update the universe
		u.update(dt);

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

		// TODO: Need some basic view manipulation
		/*
		// Center and squish into a 1x1 cube
		glScalef(1./meshScale,1./meshScale,1./meshScale);
		glTranslatef(-meshCenter[0],-meshCenter[1],-meshCenter[2]);

		// Rotate slowly around the y-axis
		glRotatef(time*20,0,1,0);
		m->drawGL();
		*/

		glPushMatrix();
		glMultMatrixf((GLfloat*) gRotationMatrix);

		BOOST_FOREACH(boost::shared_ptr<fg::Mesh> m, u.meshes()){
			m->drawGL();
		}

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

	glfwSetWindowTitle("fugu viewer");
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
	else {
		// avoid any mouse button trickery
		gMouseState.leftButtonDown = false;
	}
}
