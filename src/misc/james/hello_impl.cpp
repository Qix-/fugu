/**
 * Hello impl!!
 */


#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/functions.h"
#include "fgv/trackball.h"

#include "fg/glrenderer.h"
#include "fg/mesh.h"

// opengl viz, hack
#include "fg/meshimpl.h"
#include <wrap/gl/trimesh.h>

#include "fg/gc/generalisedcylinderimpl.h"
#include "fg/gc/impltri.h"
#include "fg/gc/lincarrier.h"
#include "fg/gc/morphcrosssec.h"
#include "fg/gc/crosssectioncircular.h"

#include "fg/gc/turtle.h"

using namespace std;
using namespace fg;
using namespace fg::gc;
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

int main(int argc, char *argv[])
{
	int DRAW_MODE = 2; // start in flat
	const int NUM_DRAW_MODES = 4;

	setupWindowAndGL();

	// Create a cylinder
	Turtle bert;

	bert.setCarrierMode(0);
	bert.beginCylinder( );
	bert.move(1.);
	bert.endCylinder();

	double uNorm, u, v = 0.;

	uNorm = 0.;
	u = bert.mCylinders[0]->unNormalise( uNorm, v );
	std::cout << "uNorm = " << uNorm << ", u = " << u << ", pos = " << bert.mCylinders[0]->getPosition( u, 0. ) << "\n";
	uNorm = 1.;
	u = bert.mCylinders[0]->unNormalise( uNorm, v );
	std::cout << "uNorm = " << uNorm << ", u = " << u << ", pos = " << bert.mCylinders[0]->getPosition( u, 0. ) << "\n";
	uNorm = .5;
	u = bert.mCylinders[0]->unNormalise( uNorm, v );
	std::cout << "uNorm = " << uNorm << ", u = " << u << ", pos = " << bert.mCylinders[0]->getPosition( u, 0. ) << "\n";

	u = 0.;
	v = .1;
	Vec3 norm = bert.mCylinders[0]->getNorm( u, v );
	Vec3 du = bert.mCylinders[0]->getDerivativeU( u, v );
	Vec3 dv = bert.mCylinders[0]->getDerivativeV( u, v ); 
	Vec3 normu = bert.mCylinders[0]->getNormU( u, v );
	Vec3 normv = bert.mCylinders[0]->getNormV( u, v );
	std::cout << "(" << u << ", " << v << ") -> " << "du = " << du << ", dv = " << dv << ", norm = " << norm << ", nu = " << normu << ", nv = " << normv << "\n";

	u = 1.;
	v = 0.;
	norm = bert.mCylinders[0]->getNorm( u, v );
	std::cout << "(" << u << ", " << v << ") -> " << norm << "\n\n\n";

	u = 0.0;
	v = 0.5;
	double f = 0.;
	Vec3 grad;
	bert.mCylinders[0]->impl( Vec3(1., 0., 0.5 ), u, v, f, grad );
	cout << "imp(1.1, 0.1, 0.1) = " << f << ", " << grad << ", u = " << u << ", v = " << v << endl;

	// ** Create new mesh here 
	GeneralisedCylinderImpl gc;
	ImplTri ip( gc );
	std::vector<Vec3> points;
	std::vector<int> tris;
	ip.triangulate( M_PI / 10., points, tris );

	Mesh::MeshBuilder mb;

	//cout << "Points:\n";
	for( int i = 0; i < points.size(); ++i )
	{
		//cout << i << ": " << points[i] << endl;
		mb.addVertex( points[i] );
	}

	//cout << "\nTriangles:\n";
	for( int i = 0; i < tris.size(); i = i + 3 )
	{
		//cout << i << ": " << tris[i] << ", " << tris[i+1] << ", " << tris[i+2] << endl;
		mb.addFace( tris[i], tris[i+1], tris[i+2] );
	}

	boost::shared_ptr<Mesh> mesh = mb.createMesh();

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
		fg::GLRenderer::renderMesh(&*mesh,fg::GLRenderer::RenderMeshMode(DRAW_MODE));
		glPointSize(5.);
		for( int i = 0; i < points.size(); ++i ) {
				glBegin( GL_POINTS );
				  glVertex3d( points[i].X(), points[i].Y(), points[i].Z() );
				glEnd( );
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

	glfwSetWindowTitle("hello carrier!");
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
	else if (button==GLFW_MOUSE_BUTTON_MIDDLE){
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

