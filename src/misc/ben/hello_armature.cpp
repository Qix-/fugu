/**
 * Armature prototype
 */

#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/util.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"
#include "fg/quat.h"
#include "fg/mat4.h"
#include "fg/vec3.h"
#include "fg/util.h"
#include "fg/armature.h"

#include "fgv/trackball.h"

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

class Demo {
public:
	virtual void setup(){}
	virtual void animate(double,double){}
	virtual void draw(){}
};

class Demo1:public Demo {
public:
	static const int NUM_BoneS = 6;

	void setup(){
		double length = 1;

		arm.root()->setLength(length);
		arm.root()->setOrientation(Quat(Vec3(0,0,1),0));

		BoneRef parent = arm.root();
		for(int i=0;i<NUM_BoneS-1;i++){
			BoneRef child = BoneRef(new Bone(parent));
			child->setPosition(Vec3(0,0,0));
			child->setOrientation(Quat(Vec3(0,0,1),0));
			child->setLength(length);
			parent->addChild(child);

			arm.addBone(child);
			parent = child;
			length *= 0.8;
		}
	}

	void animate(double time, double dt){
		int i = 0;
		foreach(BoneRef pb, arm.bones()){
			double di = (double)i/(NUM_BoneS-1);
			double da = (PI/4) * (1.4 - di);
			double ds = sin(time); // (2*(1+di))*time;
			ds *= fabs(ds);
			pb->setOrientation(Quat(Vec3(0,0,1), da*ds).normalised());
			i++;
		}
		arm.update();
	}

	void draw(){
		GLRenderer::renderArmature(arm);
	}

	Armature arm;
	//Bone* root;
	//Bone* joints[NUM_BoneS];
};

class Demo2: public Demo {
public:

	static const int DEPTH = 3;
	static const int NUM_CHILDREN = 3;
	static const int NUM_BONES = 1+3+9+27; // computed from the above

	void setup(){
		for(int i=0;i<NUM_BONES;i++) origAngleF[i] = false;

		// arm.root()->mLength = 0; // NOTE: Length is by default 0 for the root bone
		arm.root()->setOrientation(Quat(Vec3(1,0,0),Vec3(0,1,0)).normalised()); // Point upwards
		setupChildren(arm.root(), 0, 1);
	}

	void setupChildren(BoneRef parent, int depth, double length){
		if (depth >= DEPTH) return;

		const double D_ANGLE = 2*PI/NUM_CHILDREN;

		for(int i=0;i<NUM_CHILDREN;i++){
			BoneRef child = BoneRef(new Bone(parent));
			parent->addChild(child);
			arm.addBone(child);

			// (1,0,0) is the outward direction
			Vec3 dir = Vec3(1,cos(D_ANGLE*i),sin(D_ANGLE*i)).normalised();
			child->setPosition(Vec3(0,0,0));
			child->setOrientation(Quat(Vec3(1,0,0),dir).normalised()); // rotate to point in dir
			child->setLength(length);

			// recurse the downward spiral..
			setupChildren(child, depth+1, length*.666);
		}
	}

	void animate(double time, double dt){
		int i = 0;
		foreach(BoneRef pb, arm.bones()){
			if (i>0){ // ignore root

				if (not origAngleF[i]){
					origAngleF[i] = true;
					axes[i] = pb->getOrientation().getAxis();
					origAngle[i] = pb->getOrientation().getAngle();
				}

				double di = (double)i/(NUM_BONES-1);
				double da = (PI/16); // * (1.4 - di);
				double ds = time; // (2*(1+di))*time;

				pb->setOrientation(Quat(axes[i], origAngle[i] + da*sin(ds)));
			}
			i++;
		}
		arm.update();
	}

	void draw(){
		GLRenderer::renderArmature(arm);
	}

	Armature arm;
	Vec3 axes[NUM_BONES];
	double origAngle[NUM_BONES];
	bool origAngleF[NUM_BONES];
};

class Demo3: public Demo {
public:
	static const int NUM_BoneS = 6;

	static const double TPM = .5; // time per move..

	void setup(){
		mTime = TPM+1;

		double length = 1;

		arm.root()->setLength(length);
		arm.root()->setOrientation(Quat(Vec3(0,0,1),0));

		BoneRef parent = arm.root();
		for(int i=0;i<NUM_BoneS-1;i++){
			BoneRef child = BoneRef(new Bone(parent));
			child->setPosition(Vec3(0,0,0));
			child->setOrientation(Quat(Vec3(0,0,1),0));
			child->setLength(length);
			parent->addChild(child);

			arm.addBone(child);
			parent = child;
			length *= 0.8;
		}
	}

	void animate(double time, double dt){
		if (mTime > TPM){
			mTime = 0;

			int randomIndex = (int)(random(0,arm.bones().size()));
			std::list<BoneRef>::const_iterator it = arm.bones().begin();
			for(int i=0;i<randomIndex;i++) it++;
			mCurrentBone = *it;
			mInitialRotation = mCurrentBone->getOrientation();
			mTargetRotation = Quat(Vec3(0,1,0),Vec3(random(-1,1),random(-1,1),random(-1,1)));
			mTargetRotation.normalise();
		}
		else {
			mTime+=dt;
			mCurrentBone->setOrientation(mInitialRotation.slerp(smoothstep<double>(0,1,mTime/TPM),mTargetRotation));
		}
		arm.update();
	}

	void draw(){
		GLRenderer::renderArmature(arm);
	}

	Armature arm;

	BoneRef mCurrentBone;
	Quat mInitialRotation;
	Quat mTargetRotation;
	double mTime;
};

class Demo4: public Demo {
public:

	static const int DEPTH = 3;
	static const int NUM_CHILDREN = 7;
	static const int NUM_BONES = 1+7+49+343; // computed from the above

	void setup(){
		for(int i=0;i<NUM_BONES;i++) origAngleF[i] = false;

		// arm.root()->mLength = 0; // NOTE: Length is by default 0 for the root bone
		arm.root()->setOrientation(Quat(Vec3(1,0,0),Vec3(0,1,0)).normalised()); // Point upwards
		setupChildren(arm.root(), 0, 1.5);
	}

	void setupChildren(BoneRef parent, int depth, double length){
		if (depth >= DEPTH) return;

		/* from : http://www.cgafaq.info/wiki/Evenly_distributed_points_on_sphere
		dlong := pi*(3-sqrt(5))  // ~2.39996323
		dz    := 2.0/N
		long := 0
		z    := 1 - dz/2
		for k := 0 .. N-1
		    r    := sqrt(1-z*z)
		    node[k] := (cos(long)*r, sin(long)*r, z)
		    z    := z - dz
		    long := long + dlong
		*/

		double dl = PI*(3-sqrt(5));
		double dz = 2./NUM_CHILDREN;
		double l = 0;
		double z = 1-dz/2;

		for(int i=0;i<NUM_CHILDREN;i++){
			BoneRef child = BoneRef(new Bone(parent));
			parent->addChild(child);
			arm.addBone(child);

			double r = sqrt(1-z*z);
			Vec3 dir = Vec3(cos(l)*r,sin(l)*r,z).normalised();
			z = z - dz;
			l = l+dl;

			// (1,0,0) is the outward direction
			// Vec3 dir = Vec3(1,cos(D_ANGLE*i),sin(D_ANGLE*i)).normalised();
			child->setPosition(Vec3(0,0,0));
			child->setOrientation(Quat(Vec3(1,0,0),dir).normalised()); // rotate to point in dir
			child->setLength(length);

			// recurse the downward spiral..
			setupChildren(child, depth+1, length*.9);
		}
	}

	void animate(double time, double dt){

		int i = 0;
		foreach(BoneRef pb, arm.bones()){
			if (i>0){ // ignore root

				if (not origAngleF[i]){
					origAngleF[i] = true;
					axes[i] = pb->getOrientation().getAxis();
					origAngle[i] = pb->getOrientation().getAngle();
					origLength[i] = pb->getLength();
				}

				double di = (double)i/(NUM_BONES-1);
				double da = (PI/16); // * (1.4 - di);
				double ds = time; // (2*(1+di))*time;

				// pb->getOrientation()*Quat(Vec3(0,1,0),Vec3(random(0,.1),1,random(0,.1)))
				double craziness = 0.01;
				pb->setOrientation(pb->getOrientation()*Quat(Vec3(0,1,0),(Vec3(random(-1,1),1/craziness,random(-1,1))*craziness).normalized()));
				pb->setLength(origLength[i] + .1*sin(3*ds));
			}
			i++;
		}

		arm.update();
	}

	void draw(){
		GLRenderer::renderArmature(arm);
	}

	Armature arm;
	Vec3 axes[NUM_BONES];
	double origAngle[NUM_BONES];
	double origLength[NUM_BONES];
	bool origAngleF[NUM_BONES];
};


int DEMO = 0;
const int NUM_DEMOS = 4;

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
	Demo* demos[] = {new Demo1(), new Demo2(), new Demo3(), new Demo4()};
	for(int i=0;i<NUM_DEMOS;i++) demos[i]->setup();

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
		demos[DEMO]->animate(time,dt);

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
		demos[DEMO]->draw();

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
