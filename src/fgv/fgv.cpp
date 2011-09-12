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
void TW_CALL resetCamera(void *clientData);

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
	bool enableLighting;

	int numberSubdivs;

	enum MeshMode { MM_SMOOTH, MM_FLAT, MM_WIRE, MM_POINTS, MM_TEXTURED };
	MeshMode meshMode;

	fg::GLRenderer::ColourMode colourMode;
} gViewMode = {true,true,false,true,0,ViewMode::MM_SMOOTH};

enum SimulationMode {SM_PLAYING, SM_PAUSED, SM_STEPPING, SM_RELOADING, SM_ERROR};

struct AppState {
	fg::Universe* universe;
	char** argv;
	SimulationMode simulationMode;
	SimulationMode previousMode;
	double timeMultiplier;
	double time; // current time in the simulation, read from universe->time
} gAppState = {NULL, NULL, SM_PAUSED, SM_PAUSED, 1, 0};

// control callbacks
void TW_CALL playCb(void *clientData){
	if (gAppState.simulationMode==SM_ERROR) {
		// don't change mode
	}
	else
		gAppState.simulationMode = SM_PLAYING;
}
void TW_CALL pauseCb(void *clientData){
	if (gAppState.simulationMode==SM_ERROR) {
		// don't change mode
	}
	else
		gAppState.simulationMode = SM_PAUSED;
}
void TW_CALL stepCb(void *clientData){
	if (gAppState.simulationMode==SM_ERROR) {
		// don't change mode
	}
	else
		gAppState.simulationMode = SM_STEPPING;
}
void TW_CALL reloadCb(void *clientData){
	if (gAppState.simulationMode==SM_ERROR) {
		gAppState.previousMode = SM_PAUSED;
	}
	else {
		gAppState.previousMode = gAppState.simulationMode;
	}

	gAppState.simulationMode = SM_RELOADING;
}

void loadUniverse(); // load or reload universe
void deleteUniverse(); // delete the universe (probably due to an error)

void GLFWCALL keyCallback(int key, int action);
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

	// Initialise the universe
	gAppState.argv = argv;
	loadUniverse();

	bool running = true;
	double before = glfwGetTime();
	double now = glfwGetTime();

	// Run as fast as I can
	while(running){
		// compute how long this frame takes to update and draw
		before = glfwGetTime();

		double dt = SPF*gAppState.timeMultiplier;

		// Update the universe
		switch (gAppState.simulationMode)
		{
			case SM_PLAYING: {
				if (gAppState.universe!=NULL)
					try {
						gAppState.universe->update(dt);
					}
					catch (std::runtime_error& e){
						std::cerr << "ERROR: " << e.what() << "\n";
						deleteUniverse();
						gAppState.simulationMode = SM_ERROR;
					}
				break;
			}
			case SM_PAUSED: {
				// do nothing
				break;
			}
			case SM_STEPPING: {
				if (gAppState.universe!=NULL){
					try {
						gAppState.universe->update(dt);
						gAppState.simulationMode = SM_PAUSED;
					}
					catch (std::runtime_error& e){
						std::cerr << "ERROR: " << e.what() << "\n";
						deleteUniverse();
						gAppState.simulationMode = SM_ERROR;
					}
				}
				break;
			}
			case SM_RELOADING: {
				loadUniverse();
				if (gAppState.simulationMode==SM_ERROR){
					// oh noes...
					// do nothing
				}
				else { // oh yeas
					// recompute before time so we don't do a massive first step
					before = glfwGetTime();
					gAppState.simulationMode = gAppState.previousMode;
				}
				break;
			}
			case SM_ERROR: {
				// don't do anything...
				break;
			}
		}

		if (gAppState.universe!=NULL)
			gAppState.time = gAppState.universe->time();

		// Draw all the meshes in the universe
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1,1,1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0,0,4,   0,0,0,   0,1,0);

		if (gViewMode.enableLighting){
			GLfloat lp[] = {1, 1, 1, 0};
			glLightfv(GL_LIGHT0,GL_POSITION,lp);
			glEnable(GL_LIGHTING);
		}
		else {
			glDisable(GL_LIGHTING);
		}
		glPushMatrix();
		glMultMatrixf((GLfloat*) gRotationMatrix);

		glTranslatef(gCameraTranslation[0],
				gCameraTranslation[1],
				gCameraTranslation[2]);

		// GLfloat lp[] = {.1, 1, .1, 0};
		// glLightfv(GL_LIGHT0,GL_POSITION,lp);

		float z = std::exp(-gZoom);
		glScalef(z,z,z);

		if (gViewMode.origin) drawOrigin();
		if (gViewMode.ground) drawGroundPlane();
        
		if (gAppState.universe!=NULL){
			foreach(shared_ptr<fg::MeshNode> m, gAppState.universe->meshNodes()){
				// std::cout << m << "\n" << *m << "\n\n";

				m->mesh()->sync(); // make sure normals are okay
				// Subdivide the viewing mesh if required

				shared_ptr<fg::Mesh> old = shared_ptr<fg::Mesh>();
				if (gViewMode.numberSubdivs>0){
					// copy the mesh
					old = m->mesh();
					shared_ptr<fg::Mesh> clone = old->clone();
					clone->smoothSubdivide(gViewMode.numberSubdivs);
					m->setMesh(clone);
				}

				fg::GLRenderer::RenderMeshMode rmm;
				switch (gViewMode.meshMode){
					case ViewMode::MM_SMOOTH: rmm = fg::GLRenderer::RENDER_SMOOTH; break;
					case ViewMode::MM_FLAT: rmm = fg::GLRenderer::RENDER_FLAT; break;
					case ViewMode::MM_WIRE: rmm = fg::GLRenderer::RENDER_WIRE; break;
					case ViewMode::MM_POINTS: rmm = fg::GLRenderer::RENDER_VERTICES; break;
					case ViewMode::MM_TEXTURED: rmm = fg::GLRenderer::RENDER_TEXTURED; break;
				}
				fg::GLRenderer::renderMeshNode(m,rmm,gViewMode.colourMode); // fg::GLRenderer::RenderMeshMode(DRAW_MODE));

				if (gViewMode.numberSubdivs>0){
					m->setMesh(old);
				}

				// clone should get garbage collected...
			}

			if (gViewMode.showNodeAxes){
				foreach(shared_ptr<fg::Node> n, gAppState.universe->nodes()){
					glPushMatrix();
					fg::Mat4 t = n->getCompoundTransform().transpose();
					glMultMatrixd(t.V());
					fg::GLRenderer::renderAxes();
					glPopMatrix();
				}
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

	if (gAppState.universe!=NULL){
		// Clean up the old universe
		delete gAppState.universe;
		gAppState.universe = NULL;
	}

	return EXIT_SUCCESS;
}

void loadUniverse(){ // load or reload universe
	std::cout << "Loading universe\n";

	if (gAppState.universe!=NULL){
		// Clean up the old universe
		delete gAppState.universe;
		gAppState.universe = NULL;
		// TODO: make sure old slider values carry over into new state
	}

	try {
		// Create a new universe
		gAppState.universe = new fg::Universe();
		gAppState.universe->addScriptDirectory("../scripts/?.lua");
		gAppState.universe->loadScript(gAppState.argv[1]);
	}
	catch (std::runtime_error& e){
		gAppState.simulationMode = SM_ERROR;
		std::cerr << "ERROR: " << e.what() << "\n";
		if (gAppState.universe!=NULL){
			delete gAppState.universe;
			gAppState.universe = NULL;
		}
	}
}

void deleteUniverse(){
	if (gAppState.universe!=NULL){
		delete gAppState.universe;
		gAppState.universe = NULL;
	}
}

// Function called by AntTweakBar to copy the content of a std::string handled
// by the AntTweakBar library to a std::string handled by your application
void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
  destinationClientString = sourceLibraryString;
}

void setupWindowAndGL(){
	// Setup GL etc
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}

	GLFWvidmode mode;   // GLFW video mode
	// Create a window
	glfwGetDesktopMode(&mode);

	if (!glfwOpenWindow(gWidth,gHeight, mode.RedBits, mode.GreenBits, mode.BlueBits, 0, 24, 0, GLFW_WINDOW)){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwEnable(GLFW_MOUSE_CURSOR);
	glfwEnable(GLFW_KEY_REPEAT);

	GLenum err = glewInit();
	if (GLEW_OK != err){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle("fg viewer");

	// Window is loaded, set up gui
	TwInit(TW_OPENGL, NULL);
	TwCopyStdStringToClientFunc(CopyStdStringToClient); // must be called once (just after TwInit for instance)

	TwBar* mainBar = TwNewBar("fgmenu");
	TwDefine(" fgmenu label='fg menu' refresh=0.2 ");
    TwAddButton(mainBar, "cam reset", resetCamera, NULL," label='Reset Camera'");
	TwAddVarRW(mainBar, "origin", TW_TYPE_BOOLCPP, &gViewMode.origin,
	               " group='View' help='Toggle origin.' ");
	TwAddVarRW(mainBar, "ground", TW_TYPE_BOOLCPP, &gViewMode.ground,
	               " group='View' help='Toggle ground.' ");
	TwAddVarRW(mainBar, "node axes", TW_TYPE_BOOLCPP, &gViewMode.showNodeAxes,
		               " group='View' help='Toggle node axes.' ");

	TwAddVarRW(mainBar, "lighting", TW_TYPE_BOOLCPP, &gViewMode.enableLighting,
			" group='View' help='Enable lighting.' ");

	TwAddVarRW(mainBar, "smooth subdiv", TW_TYPE_INT32, &gViewMode.numberSubdivs,
			" min=0 max=3 group='View' help='Turn on smooth subdivision for visualisation.' ");

	TwEnumVal mmEV[] = { { ViewMode::MM_SMOOTH, "Smooth"},
			{ ViewMode::MM_FLAT, "Flat"},
			{ ViewMode::MM_WIRE, "Wireframe"},
			{ ViewMode::MM_POINTS, "Points"},
			{ ViewMode::MM_TEXTURED, "Textured"}
	};

	TwType mmType = TwDefineEnum( "Mesh Mode", mmEV, 5 );
	TwAddVarRW(mainBar, "mesh mode", mmType, &gViewMode.meshMode,
			" group='View' keyIncr=Tab keyDecr=SHIFT+Tab help='Change the mesh rendering mode.' ");

	TwEnumVal mmEVCM[] = {
			{fg::GLRenderer::COLOUR_NONE, "None"},
			{fg::GLRenderer::COLOUR_VERTEX, "Vertex"},
			{fg::GLRenderer::COLOUR_FACE_MANIFOLD, "Face (Is Manifold?)"},
	};

	TwType cmType = TwDefineEnum("Colour Mode", mmEVCM, 3);
	TwAddVarRW(mainBar, "colour mode", cmType, &gViewMode.colourMode,
				" group='View' help='Change the mesh colouring mode.' ");

	// ** CONTROL GROUP
	TwAddButton(mainBar, "play", playCb, NULL, " group='Control' ");
	TwAddButton(mainBar, "pause", pauseCb, NULL, " group='Control' ");
	TwAddButton(mainBar, "step", stepCb, NULL, " group='Control' ");
	TwAddButton(mainBar, "reload", reloadCb, NULL, " group='Control' ");

	TwEnumVal mmEVSM[] = {
			{SM_PLAYING, "Playing"},
			{SM_PAUSED, "Paused"},
			{SM_STEPPING, "Stepping"},
			{SM_RELOADING, "Reloading"},
			{SM_ERROR, "Error"},
	};
	TwType simType = TwDefineEnum("Simulation Mode", mmEVSM, 5);
	TwAddVarRO(mainBar, "sim mode", simType, &gAppState.simulationMode,
			" group='Control' help='The simulation mode' ");
	TwAddVarRO(mainBar, "time", TW_TYPE_DOUBLE, &gAppState.time,
			" group='Control' help='Current simulation time' ");
	TwAddVarRW(mainBar, "time mult", TW_TYPE_DOUBLE, &gAppState.timeMultiplier,
			" group='Control' help='Speed the simulation up or down' min=0 max=5 step=.1 " );



	// ** STATUS BAR
	// ANTTWEAKBAR doesn't like std::string?
	/*
	TwBar* statusBar = TwNewBar("status");
	std::ostringstream oss;
	oss << " status position='" << 30 << " " << gHeight - 80 << "' "
			<< "size='" << gWidth-60 << " " << 20 << "' ";
	TwDefine(strdup(oss.str().c_str()));

	std::string* error = new std::string("An error message this is.");
	TwAddVarRO(statusBar, "err", TW_TYPE_STDSTRING, error, "");
	// TwAddVarRO(statusBar, "err2", TW_TYPE_STDSTRING, &error2, "");
	TwAddButton(statusBar, "play", playCb, NULL, "");
	*/

	// ** Redirect GLFW CALLBACKS
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

	// glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void GLFWCALL keyCallback(int key, int action)
{
	if (not TwEventKeyGLFW(key,action)){
		// custom stuff...
	}
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
        // along x-axis (left-right) in view plane...
        gCameraTranslation[0] += 2*(dX-dLastX) * gRotationMatrix[0][0];
		gCameraTranslation[1] += 2*(dX-dLastX) * gRotationMatrix[1][0];
        gCameraTranslation[2] += 2*(dX-dLastX) * gRotationMatrix[2][0];
        
        // along y-axis (up-down) in view plane
        gCameraTranslation[0] += 2*(dY-dLastY) * gRotationMatrix[0][1];
		gCameraTranslation[1] += 2*(dY-dLastY) * gRotationMatrix[1][1];
        gCameraTranslation[2] += 2*(dY-dLastY) * gRotationMatrix[2][1];

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
	glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(2);
	fg::GLRenderer::renderAxes();
	glPopAttrib();
}

void drawGroundPlane(){
	glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
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
}

/** reset camera transformations
 */
void TW_CALL resetCamera(void *clientData)
{
    gZoom = 0;
    gRotationQuat[0] = 0, gRotationQuat[1] = 0, gRotationQuat[2] = 0, gRotationQuat[3] = 1;
    for (int i = 0; i < 4; ++i)
    {
        gRotationQuat[i] = (i == 3 ? 1 : 0);
        for(int j = 0; j < 4; ++j)
            gRotationMatrix[i][j] = (i == j ? 1 : 0);
    }
    gCameraTranslation[0] = gCameraTranslation[1] = gCameraTranslation[2] = 0;
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




















