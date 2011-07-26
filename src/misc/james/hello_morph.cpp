/**
 * Hello gc!
 */


#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/functions.h"
#include "fgv/trackball.h"

#include "fg/lincarrier.h"
#include "fg/pbezcarrier.h"
#include "fg/interpcrosssec.h"
#include "fg/morphcrosssec.h"
#include "fg/scaleinterpcrosssec.h"
#include "fg/generalisedcylinder.h"
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

	// ** Create new carrier here
	const int numPoints = 2;
	std::vector<Mat4> arr;
	std::vector<Vec3> arr2;
	std::vector<Vec3> arr3;
	std::vector<Vec3> arr4;
	std::vector< std::pair<Vec3,Vec3> > grad;
	std::vector< std::pair<Vec3,Vec3> > grad2;
	std::vector< std::pair<Vec3,Vec3> > grad3;
	Mat4 tmp;

	arr.push_back( Mat4() );
	arr[0].SetIdentity();

	arr.push_back( Mat4() );
	arr[1].SetTranslate(0.,0.,1.);

    arr2.push_back( Vec3(.1, 0., 0.) );
    arr2.push_back( Vec3(.0, .1, 0.) );
    arr2.push_back( Vec3(-.1, 0., 0.) );
    arr2.push_back( Vec3(.0, -.1, 0.) );

	grad.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,.055,0.),Vec3(.0,.055,0.)));
	grad.push_back( std::pair<Vec3,Vec3> ( Vec3(-.055,.0,0.),Vec3(-.055,.0,0.)));
	grad.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,-.055,0.),Vec3(.0,-.055,0.)));
	grad.push_back( std::pair<Vec3,Vec3> ( Vec3(.055,.0,0.),Vec3(.055,.0,0.)));

    arr3.push_back( Vec3(.13, .0, 0.) );
    arr3.push_back( Vec3(.0, .13, 0.) );
    arr3.push_back( Vec3(-.13, .0, 0.) );

	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.,0.),Vec3(0.,0.,0.)));
	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.,0.),Vec3(0.,0.,0.)));
	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.,0.),Vec3(0.,.0,0.)));
	/*
    arr3.push_back( Vec3(.1, -0.1, 0.) );
    arr3.push_back( Vec3(.1, 0.1, 0.) );
    arr3.push_back( Vec3(-.1, .1, 0.) );
    arr3.push_back( Vec3(-.1, -.1, 0.) );

	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.0,0.0,0.),Vec3(0.0,0.0,0.)));
	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,0.0,0.),Vec3(0.,0.0,0.)));
	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(-0.0,0.,0.),Vec3(-0.0,0.,0.)));
	grad2.push_back( std::pair<Vec3,Vec3> ( Vec3(0.,-0.0,0.),Vec3(0.,-0.0,0.)));
	*/

    arr4.push_back( Vec3(.4, 0., 0.) );
    arr4.push_back( Vec3(.02, 0.02, 0.) );
    arr4.push_back( Vec3(.0, 0.4, 0.) );
    arr4.push_back( Vec3(-.02, 0.02, 0.) );
    arr4.push_back( Vec3(-.4, .0, 0.) );
    arr4.push_back( Vec3(-.02, -0.02, 0.) );
    arr4.push_back( Vec3(.0, -0.4, 0.) );
    arr4.push_back( Vec3(.02, -0.02, 0.) );

	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,.0,0.),Vec3(.0,.0,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(-.0,.0,0.),Vec3(-.0,.0,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(-.0,0.,0.),Vec3(-0.0,0.,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,0.,0.),Vec3(.0,0.,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,0.,0.),Vec3(.0,0.,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,0.,0.),Vec3(.0,0.,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,0.,0.),Vec3(.0,0.,0.)));
	grad3.push_back( std::pair<Vec3,Vec3> ( Vec3(.0,0.,0.),Vec3(.0,0.,0.)));

//	arr.push_back( Mat4() );
//	arr[2].SetTranslate(0.,1.,2.);
//	tmp.SetRotateDeg(-45, Vec3( 1., 0., 0.));
//	arr[2] = arr[2] * tmp;

	std::vector< std::pair<double,double> > domains;
	domains.push_back( std::pair<double,double>( 0., 0. ) );
	domains.push_back( std::pair<double,double>( 1., 1. ) );

	const fg::gc::CarrierCurve& carrier = gc::PBezCarrier(arr);

    std::vector< fg::spline::PBezInterpDiv > interps;
	fg::spline::PBezInterpDiv interp;
    interp = fg::spline::PBezInterpDiv (arr2, grad);

	interps.push_back( interp );
	interps.push_back( fg::spline::PBezInterpDiv (arr3, grad2) );
	interps.push_back( fg::spline::PBezInterpDiv (arr4, grad3) );

	interps[0].setOpen( false );
	interps[1].setOpen( false );
	interps[2].setOpen( false );

	const fg::gc::MorphCrossSec cs( interps );
	//const fg::gc::GeneralisedCylinder& gc = gc::GeneralisedCylinder(carrier, cs, arr, domains);
	//Mesh::MeshBuilder mb;
	//gc.createMesh(mb, 10, 5);
	//boost::shared_ptr<Mesh> mMesh = mb.createMesh();
	//mMesh->smoothSubdivide(2);

	// Run as fast as I can
	bool running = true;

	double time = glfwGetTime();
	double dt = 0.01;

    float v = 0.;
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
		fg::GLRenderer::renderInterpolator(cs.getCrossSectionInterp( v ), 20);
		if( time > 1. && v < interps.size() - 1 )
			v += 0.0007;

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

	glfwSetWindowTitle("hello morph!");
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
