/**
 * Armature prototype
 */

#include <iostream>
#include <cmath>
#include "GL/glew.h"
#include "GL/glfw.h"

#include "fg/fg.h"
#include "fg/util.h"
#include "fg/mesh.h"
#include "fg/meshimpl.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"
#include "fg/quat.h"
#include "fg/mat4.h"
#include "fg/vec3.h"
#include "fg/util.h"
#include "fg/armature.h"
#include "fg/turtle.h"
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

const double height = 3;
const int numBones = 10;
const int numMeshSegments = 7;

enum DrawMode {MESH_ONLY=0, ARMATURE_ONLY, BOTH, NUM_DRAW_MODES};
int drawMode = MESH_ONLY;

class Demo1:public Demo {
public:

	void setup(){

		gc::Turtle bert;
		bert.setFrame(Vec3(0,0,0),Vec3(0,1,0),Vec3(0,0,1));
		double sc = 2;
		bert.setScale(sc);
		bert.beginCylinder();
		for(int i=0;i<numMeshSegments;i++){
			double di = (double)i/numMeshSegments;
			bert.move(height/numMeshSegments);
			bert.setScale(lerp(sc,random(.1,.2),di));
			bert.addPoint();
		}
		bert.endCylinder();
		mesh = bert.getMesh(8,100);

		BoneWeakRef bones[numBones];

		double length = height/numBones;
		arm.root()->setLength(length);
		arm.root()->setInitialPosition(Vec3(0,0,0));
		arm.root()->setInitialOrientation(Quat(Vec3(1,0,0),Vec3(0,1,0)));
		bones[0] = arm.root();

		BoneRef parent = arm.root();
		for(int i=1;i<numBones;i++){
			BoneRef child = BoneRef(new Bone(parent));
			bones[i] = child;

			child->setInitialPosition(Vec3(0,0,0));
			child->setInitialOrientation(Quat(Vec3(1,0,0),0));
			child->setLength(length);
			parent->addChild(child);
			arm.addBone(child);

			parent = child;
		}

		/* do a custom binding vertices to this N-bone armature */

		double jointRadius = length/4;

		arm.root()->computeInitialWorldSpaceTransforms();
		foreach(VertexImpl& vi, mesh->_impl()->vert){
			double y = vi.P().Y();

			// map y onto the bone index it corresponds to
			int bi = std::floor(y/length);
			bi = fg::clamp(bi,0,numBones-1);
			// vi.C().Y() = (255.*bi/(numBones-1)); // map g channel to bone

			double dy = y - bi*length; // dy is between 0 and length
			// vi.C().Y() = 255*(dy/length);

			if (dy < jointRadius){
				if (bi==0){
					vi.bindBone(bones[bi],1);
				}
				else {
					// bound to last bone and this bone
					double dr = (dy+jointRadius)/(2*jointRadius);
					vi.bindBone(bones[bi-1],1-dr);
					vi.bindBone(bones[bi],dr);
					// vi.C().Y() = dr*255;
				}
			}
			else if (dy > (length-jointRadius)){
				if (bi==(numBones-1)){
					vi.bindBone(bones[bi],1);
				}
				else {
					// bound to next bone and this bone
					dy -= length;
					double dr = (dy+jointRadius)/(2*jointRadius);
					vi.bindBone(bones[bi],1-dr);
					vi.bindBone(bones[bi+1],dr);
					//vi.C().Y() = (1-dr)*255;
				}
			}
			else {
				vi.bindBone(bones[bi],1);
			}


			/*
			if (y<=-jointRadius){
				vi.bindBone(arm.root(),1);
				vi.C().Y() = 0;
			}
			else if (y>=-jointRadius and y <= jointRadius){
				double dr = (y+jointRadius)/(2*jointRadius);
				vi.bindBone(arm.root(),1-dr);
				vi.bindBone(child,dr);
				vi.C().Y() = dr*255;
			}
			else {
				vi.bindBone(child,1);
				vi.C().Y() = 255;
			}
			*/
		}
	}

	void animate(double time, double dt){
		int i = 0;
		time = .5*time; // slow down time
		foreach(BoneRef pb, arm.bones()){
			double di = (double)i/(arm.bones().size());
			double da = (PI/4) * (1.4 - di*di);
			double ds = sin(time+.1*di); // (2*(1+di))*time;
			ds *= fabs(ds);
			pb->setCurrentOrientation(
					pb->getInitialOrientation()*
					Quat(Vec3(0,0,1), da*ds).normalised()*
					Quat(Vec3(0,1,0), da*da*sin(7*time)).normalised());
			i++;
		}
		arm.update();

		// now update the positions of the vertices
		// using basic vertex blending
		foreach(VertexImpl& vi, mesh->_impl()->vert){
			// vi.P()bindBone(arm.root(),1);
			if (vi.getNumBones()>0)
			{
				Vec3 p = Vec3::Zero();
				for(int i=0;i<vi.getNumBones();i++){
					BoneWeakRef bwr = vi.getBone(i);
					BoneRef br = bwr.lock();
					p += br->getCWSTransform() * br->getInvIWSTransform() * vi.getOriginalPosition() * vi.getBoneWeight(i);
				}
				vi.P() = p;
			}
		}
	}

	void draw(){
		switch(drawMode){
			case MESH_ONLY:
				GLRenderer::renderMesh(mesh,GLRenderer::RENDER_SMOOTH);
				break;

			case ARMATURE_ONLY:
				GLRenderer::renderArmature(arm, true);
				break;

			case BOTH:
				GLRenderer::renderArmature(arm, false);
				glDisable(GL_LIGHTING);
				GLRenderer::renderMesh(mesh,GLRenderer::RENDER_WIRE);
				glEnable(GL_LIGHTING);
				break;
		}

	}

	shared_ptr<Mesh> mesh;
	Armature arm;
};

int DEMO = 0;
const int NUM_DEMOS = 1;

/************************************************/

void GLFWCALL keyCallback(int key, int action)
{
	if (key==GLFW_KEY_TAB and action==GLFW_PRESS){
		drawMode = (drawMode+1)%NUM_DRAW_MODES;
	}
}
int main(int argc, char *argv[])
{
	std::cout << "Tab cycles draw mode\n";
	setupWindowAndGL();

	//
	Demo1 demo1;
	demo1.setup();

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
		demo1.animate(time,dt);

		/**/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1,1,1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(2,2,2,   0,0,0,   0,1,0);

		GLfloat lp[] = {.5, 1, .5, 0};
		glLightfv(GL_LIGHT0,GL_POSITION,lp);

		glPushMatrix();
		glMultMatrixf((GLfloat*) gRotationMatrix);

		float z = std::exp(-gZoom);
		glScalef(z,z,z);

		GLRenderer::renderAxes();
		demo1.draw();

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
