/**
 * Joints prototype
 */

#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"
#include "fg/quat.h"
#include "fg/mat4.h"
#include "fg/vec3.h"

#include "fgv/trackball.h"

#include <boost/foreach.hpp>

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


void GLFWCALL resizeWindow(int width, int height);
void setupWindowAndGL();

void mouseMoved(int x, int y);
void mouseButtoned(int button, int state);

using namespace fg;

/*************** JointS *********************/

class Joint {
	public:

	Joint(Joint* parent = NULL);

	/** \brief recursively calculate world-space transform for self and descendants
	 *
	 * The update order for Joints is top-down (parent first, then children)
	 */
	void update();

	void drawSelfAndChildrenGL();

	// protected:
	Joint* mParent;
	std::list<Joint*> mChildren;

	// relative to parent
	Vec3 mRelPosition;
	Quat mRelOrientation;

	// derived
	Mat4 mRelTransform;

	// computed worldspace transform
	Mat4 mWorldSpaceTransform;
};

Joint::Joint(Joint* parent)
:mParent(parent)
,mRelPosition(0,0,0)
,mRelOrientation(Vec3(0,1,0),0)
,mWorldSpaceTransform()
{

}

void Joint::update(){
	mRelTransform.setTranslate(mRelPosition);
	mRelTransform = mRelTransform * mRelOrientation.toMat4();

	if (mParent==NULL){
		mWorldSpaceTransform = mRelTransform;
	}
	else {
		mWorldSpaceTransform = mParent->mWorldSpaceTransform * mRelTransform;
	}

	BOOST_FOREACH(Joint* b, mChildren){
		b->update();
	}
}

void Joint::drawSelfAndChildrenGL(){
	const float JOINT_RADIUS = .1;

	glColor3f(1,1,1);

	glPushMatrix();
	Mat4 tr = mWorldSpaceTransform.transpose();
	glMultMatrixd(tr.V());
	GLRenderer::glutSolidSphere(JOINT_RADIUS,8,8);
	glPopMatrix();

	BOOST_FOREACH(Joint* b, mChildren){
		// Draw a "Joint" between the joints
		Vec3 start = mWorldSpaceTransform*Vec3(0,0,0);
		Vec3 end = b->mWorldSpaceTransform*Vec3(0,0,0);
		GLRenderer::renderBone(start,end);
		b->drawSelfAndChildrenGL();
	}
}

class Demo1 {
public:
	static const int NUM_JointS = 10;

	void setup(){
		root = new Joint();
		joints[0] = root;
		Joint* parent = root;
		double length = 1;
		for(int i=0;i<NUM_JointS-1;i++){
			Joint* child = new Joint(parent);
			child->mRelPosition = Vec3(length,0,0);
			child->mRelOrientation.set(Vec3(0,0,1),0);
			parent->mChildren.push_back(child);

			joints[i+1] = child;
			parent = child;
			length *= 0.8;
		}
	}

	void animate(double time, double dt){
		for (int i=0;i<NUM_JointS;i++){
			double di = (double)i/(NUM_JointS-1);
			double da = (PI/4) * (1.4 - di);
			double ds = time; // (2*(1+di))*time;
			joints[i]->mRelOrientation.set(Vec3(0,0,1), da*sin(ds));
			joints[i]->mRelOrientation.normalise();
		}
		root->update();
	}

	void draw(){
		//glPushAttrib(GL_LIGHTING_BIT);
		//glDisable(GL_LIGHTING);
		glColor3f(1,1,1);
		root->drawSelfAndChildrenGL();
		//glPopAttrib();
	}

	Joint* root;
	Joint* joints[NUM_JointS];
};

class Demo2 {
public:
	static const int DEPTH = 3;
	static const int NUM_CHILDREN = 3;
	static const int NUM_JOINTS = 1+3+9+27; // computed from the above

	static int _index;
	void setup(){
		_index = 0;

		for(int i=0;i<NUM_JOINTS;i++) origAngleF[i] = false;

		root = new Joint();
		joints[_index++] = root;
		setupChildren(root, 0, 1);
	}

	void setupChildren(Joint* parent, int depth, double length){
		if (depth >= DEPTH) return;

		double da = 2*PI/NUM_CHILDREN;

		for(int i=0;i<NUM_CHILDREN;i++){
			Joint* child = new Joint(parent);

			Vec3 pos = Vec3(cos(da*i),1,sin(da*i)).normalised();
			child->mRelPosition = pos*length;

			child->mRelOrientation.set(Vec3(0,1,0),child->mRelPosition);
			child->mRelOrientation.normalise();

			/*
			std::cout << "[" << i << "]\n" << child->mRelOrientation << ", ";
			std::cout << child->mRelOrientation.getAxis() << "\n";
			*/
			// std::cout << child->mRelOrientation << "\n";

			parent->mChildren.push_back(child);

			joints[_index++] = child;
			setupChildren(child, depth+1, length*0.7);
		}
	}

	void animate(double time, double dt){
		for (int i=0;i<NUM_JOINTS;i++){
			if (not origAngleF[i]){
				origAngleF[i] = true;
				axes[i] = joints[i]->mRelOrientation.getAxis();
				origAngle[i] = joints[i]->mRelOrientation.getAngle();
				//std::cout << "origAngle[" << i << "] = " << origAngle[i] << "\n";
			}

			double di = (double)i/(NUM_JOINTS-1);
			double da = (PI/16); // * (1.4 - di);
			double ds = time; // (2*(1+di))*time;

			// Vec3 ax = joints[i]->mRelOrientation.getAxis();
			joints[i]->mRelOrientation.set(axes[i], origAngle[i] + da*sin(ds));
			//joints[i]->mRelOrientation.normalise();

			if (i>=0 && i<3){
				//std::cout << "[" << i << "] " << ax << " ## " << origAngle[i] + da*sin(ds) << "\n";
			}
		}
		root->update();
	}

	void draw(){
		//glPushAttrib(GL_LIGHTING_BIT);
		//glDisable(GL_LIGHTING);
		glColor3f(1,1,1);
		root->drawSelfAndChildrenGL();
		//glPopAttrib();
	}

	Joint* root;
	Joint* joints[NUM_JOINTS];
	Vec3 axes[NUM_JOINTS];
	double origAngle[NUM_JOINTS];
	bool origAngleF[NUM_JOINTS];
};

int Demo2::_index = 0;

int DEMO = 0;
const int NUM_DEMOS = 2;

/************************************************/

void GLFWCALL keyCallback(int key, int action)
{
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		DEMO = (DEMO+1)%NUM_DEMOS;
	}
}
int main(int argc, char *argv[])
{
	setupWindowAndGL();
	std::cout << "[Tab] cycles through demos...\n";

	//
	Demo1 demo1; demo1.setup();
	Demo2 demo2; demo2.setup();

	// Run as fast as I can
	bool running = true;

	double time = glfwGetTime();
	double dt = 0.01;

	while(running){
		// Recompute delta t
		double now = glfwGetTime();
		dt = now - time;
		time = now;

		/** animate **/
		if (DEMO==0) demo1.animate(time,dt);
		else if (DEMO==1) demo2.animate(time,dt);

		/**/

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

		GLRenderer::renderAxes();
		if (DEMO==0) demo1.draw();
		else if (DEMO==1) demo2.draw();

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

	glfwSetWindowTitle("bag o' bones");
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
