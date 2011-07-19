/**
 * A simple mesh viewer app. Used to test the fg::Mesh class.
 */

#include "fg/fg.h"

#include <boost/shared_ptr.hpp>

#include "GL/glew.h"
#include "GL/glfw.h"

#include <cmath>

int width = 800;
int height = 600;

void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

int main(int argc, char *argv[])
{
	setupWindowAndGL();

	// Create a mesh
	boost::shared_ptr<fg::Mesh> m = fg::Mesh::Primitives::Icosahedron();

	// Calculate its bounds
	double minx,miny,minz,maxx,maxy,maxz;
	m->getBounds(minx,miny,minz,maxx,maxy,maxz);
	double meshDims[] = {maxx-minx,maxy-miny,maxz-minz};
	double meshScale = std::max(meshDims[0],std::max(meshDims[1],meshDims[2]));
	double meshCenter[] = {(maxx-minx)/2,(maxy-miny)/2,(maxz-minz)/2};

	// Show the mesh forever more...
	bool running = true;
	while(running){
		double time = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1,1,1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(2,2,2,   0,0,0,   0,1,0);

		GLfloat lp[] = {.1, 1, .1, 0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		// Center and squish into a 1x1 cube
		glScalef(1./meshScale,1./meshScale,1./meshScale);
		glTranslatef(-meshCenter[0],-meshCenter[1],-meshCenter[2]);

		// Rotate slowly around the y-axis
		glRotatef(time*20,0,1,0);

		m->drawGL();
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

	if (!glfwOpenWindow(width,height, 0,0,0,0,8,0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowSizeCallback(resizeWindow);

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
/*
	GLfloat global_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,10);

	GLfloat col[] = {1.f,1.f,1.f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col);
	GLfloat white[] = {1.f,1.f,1.f,0.f};
	GLfloat spec[] = {.5f,.5f,.5f,0.f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
	*/
}

void GLFWCALL resizeWindow(int _width, int _height){
	width = _width;
	height = _height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0*width/height, 0.1, 100);
	glViewport(0,0,width,height);
	glMatrixMode(GL_MODELVIEW);
}
