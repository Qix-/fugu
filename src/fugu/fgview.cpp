/**
 * \file
 * \brief The widget that displays a fugu universe
 * \author ben
  *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#include <GL/glew.h>

#include <QtGui>
//in Apple if glew.h is include not necéssary include QtOpenGl. If QtOpenGl include error on some function that will not be set correctly as glActiveTexture
//By doing this may have suddenly activate ENABLE_SSAO.
#ifndef _APPLE
#include <QtOpenGL>
#endif
#include <QGLShaderProgram>

#ifdef ENABLE_SSAO
	#include <QGLFramebufferObject>
#endif

#include <QSettings>

#include <cmath>
#include <cstring>

#include "fgview.h"

#include "fg/functions.h"

#include "fg/glrenderer.h"


#include "fg_config.h"

#include "trackball.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


#define CHECK_FOR_GL_ERROR() checkForOpenGLError(__LINE__)

FGView::FGView(QWidget *parent)
:QGLWidget(parent) // NOTE: GLformat set in main.cpp
,mUniverse(NULL) // Very important to intialise as null
,mSaveSettings(true)
,mAOShader(NULL)
#ifdef ENABLE_SSAO
	,mFBO(NULL)
#endif
,mSSAO(true)
,mDepthTex(0)
,mShadersAvailable(false)
{

	// set camera defaults
	mZoom = 0;
	resetCamera();

	// mouse state
	mMouseState.lastX = 0;
	mMouseState.lastY = 0;
	mMouseState.isRotating = false;
	mMouseState.isTranslating = false;
	mMouseState.isZooming = false;

	QSettings settings("MonashUniversity", "Fugu");

	// view mode
	mOrigin = settings.value("view/origin",false).toBool();
	mGround = settings.value("view/ground",false).toBool();;
	mShowNodeAxes = settings.value("view/showNodeAxes",false).toBool();
	mLighting = settings.value("view/lighting",true).toBool();
	mShowOverWire = settings.value("view/showOverWire",true).toBool();

	// these are set in mainwindow
	mNumberSubdivs = 0;
	mMeshMode = MM_FLAT;
	mColourMode = fg::GLRenderer::COLOUR_VERTEX;

	// theme...
	mBackgroundHorizon = settings.value("view/bghorizon", QColor(79,79,79)).value<QColor>();
	mBackgroundSky = settings.value("view/bgsky", QColor(16,16,16)).value<QColor>();

	// set texture path
	fg::GLRenderer::setTexturePath((QCoreApplication::applicationDirPath() + "/" + QString(FG_BASE_LOCATION) + "assets/UV.ppm").toStdString());
}

FGView::~FGView()
{
	if (mSaveSettings){
		QSettings settings("MonashUniversity", "Fugu");

		settings.setValue("view/origin",mOrigin);
		settings.setValue("view/ground",mGround);
		settings.setValue("view/showNodeAxes",mShowNodeAxes);
		settings.setValue("view/lighting",mLighting);
		settings.setValue("view/showOverWire",mShowOverWire);

		settings.setValue("view/bghorizon", mBackgroundHorizon);
		settings.setValue("view/bgsky", mBackgroundSky);
	}
}

QSize FGView::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize FGView::sizeHint() const
{
	return QSize(400, 400);
}

void FGView::setUniverse(fg::Universe* u){mUniverse = u; update();}
void FGView::unsetUniverse(){mUniverse = NULL; update();}

QColor FGView::getBackgroundHorizonColour() const {
	return mBackgroundHorizon;
}

QColor FGView::getBackgroundSkyColour() const {
	return mBackgroundSky;
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void FGView::toggleOrigin(bool show){
	mOrigin = show;
	update();
}

void FGView::toggleGround(bool show){
	mGround = show;
	update();
}

void FGView::toggleShowNodeAxes(bool show){
	mShowNodeAxes = show;
	update();
}

void FGView::toggleShowOverWire(bool show){
	mShowOverWire = show;
	update();
}

void FGView::toggleLighting(bool on){
	mLighting = on;
	update();
}

void FGView::toggleSSAO(bool on){
#ifdef ENABLE_SSAO
	mSSAO = on;
	update();
#endif
}

void FGView::setNumberOfSubdivs(int num){
	mNumberSubdivs = num;
	update();
}

void FGView::setSubdivs0(){
	setNumberOfSubdivs(0);
}
void FGView::setSubdivs1(){
	setNumberOfSubdivs(1);
}
void FGView::setSubdivs2(){
	setNumberOfSubdivs(2);
}
void FGView::setSubdivs3(){
	setNumberOfSubdivs(3);
}


void FGView::setDrawSmooth(){
	mMeshMode = MM_SMOOTH;
	update();
}

void FGView::setDrawFlat(){
	mMeshMode = MM_FLAT;
	update();
}

void FGView::setDrawWire(){
	mMeshMode = MM_WIRE;
	update();
}
void FGView::setDrawPoints(){
	mMeshMode = MM_POINTS;
	update();
}
void FGView::setDrawTextured(){
	mMeshMode = MM_TEXTURED;
	update();
}
void FGView::setDrawPhong(){
	mMeshMode = MM_PHONG;
	update();
}

void FGView::setColourModeNone(){
	mColourMode = fg::GLRenderer::COLOUR_NONE;
	update();
}

void FGView::setColourModeVertex(){
	mColourMode = fg::GLRenderer::COLOUR_VERTEX;
	update();
}

void FGView::setBackgroundHorizonColour(QColor c){
	mBackgroundHorizon = c;
	update();
}

void FGView::setBackgroundSkyColour(QColor c){
	mBackgroundSky = c;
	update();
}

void FGView::initializeGL()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  std::cerr << "Error: " << glewGetErrorString(err) << "\n";
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";

	qglClearColor(QColor(0,0,0));

	//glEnable(GL_CULL_FACE);
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

	CHECK_FOR_GL_ERROR();

	mShadersAvailable = QGLShaderProgram::hasOpenGLShaderPrograms(context());

	if (mShadersAvailable){
		// load the shaders
		mPhongShader = loadShader("phong_vert.glsl","phong_frag.glsl");
		if (mPhongShader){
			mPhongShader->bind();
			mPhongShader->setUniformValue("shininess",(GLfloat)5);
		}
		CHECK_FOR_GL_ERROR();

		mOverWireShader = loadShader("overwire_vert.glsl","passthru_frag.glsl");
		CHECK_FOR_GL_ERROR();

#ifdef ENABLE_SSAO
		mAOShader = loadShader("ao_vert.glsl", "ao_frag.glsl");
		CHECK_FOR_GL_ERROR();
#endif

		/*
		mSubdivisionShader = new QGLShaderProgram(context());
		QFile subdivV("../assets/shaders/subdivision_vs.glsl");
		QFile subdivG("../assets/shaders/subdivision_gs.glsl");
		QFile subdivF("../assets/shaders/subdivision_fs.glsl");
		if (subdivV.open(QFile::ReadOnly | QFile::Text)
			and subdivG.open(QFile::ReadOnly | QFile::Text)
			and subdivF.open(QFile::ReadOnly | QFile::Text))
		{
			mSubdivisionShader->addShaderFromSourceCode(QGLShader::Vertex, subdivV.readAll());
			mSubdivisionShader->addShaderFromSourceCode(QGLShader::Geometry, subdivG.readAll());
			mSubdivisionShader->addShaderFromSourceCode(QGLShader::Fragment, subdivF.readAll());
			mSubdivisionShader->setGeometryInputType ( GL_TRIANGLES );
			mSubdivisionShader->setGeometryOutputType ( GL_TRIANGLE_STRIP );
			mSubdivisionShader->link();
		}
		else {
			std::cerr << "Couldn't load subdivision shader\n";
		}
		*/

#ifdef ENABLE_SSAO
		int w = width(), h = height();

		if (not QGLFramebufferObject::hasOpenGLFramebufferObjects()){
			std::cerr << "System doesn't support framebuffer objects";
		}
		else if (!GLEW_EXT_framebuffer_object or glGetFramebufferAttachmentParameterivEXT==NULL)
		{
			std::cerr << "Can't find EXT_framebuffer_object!\n";
		}
		else {
			mFBO = new QGLFramebufferObject(w,h,QGLFramebufferObject::Depth);
			if (!mFBO->isValid()){
				std::cerr << "Framebuffer invalid\n";
				delete mFBO;
				mFBO = NULL;
			}
			CHECK_FOR_GL_ERROR();
		}
#endif

	}
}

void FGView::paintGL()
{
#ifdef ENABLE_SSAO
	if (mShadersAvailable and mSSAO and mFBO){
		mFBO->bind();
		if (!mFBO->isBound()){
			std::cerr << "Couldn't bind framebuffer";
		}
	}
#endif

	CHECK_FOR_GL_ERROR();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0,0,4,   0,0,0,   0,1,0);

	glPushMatrix();
	{
		glMultMatrixf((GLfloat*) mRotationMatrix);
		glTranslatef(mCameraTranslation[0],
				mCameraTranslation[1],
				mCameraTranslation[2]);

		// draw encapsulating sphere...
		glPushAttrib(GL_DEPTH_BUFFER_BIT);
		glPushAttrib(GL_LIGHTING_BIT);
		glPushAttrib(GL_TEXTURE_BIT);
		glPushAttrib(GL_ENABLE_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(GL_FALSE);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		fg::Vec3 hc = fg::Vec3(mBackgroundHorizon.red()/255.,
				mBackgroundHorizon.green()/255.,
				mBackgroundHorizon.blue()/255.);
		fg::Vec3 tc = fg::Vec3(mBackgroundSky.red()/255.,
						mBackgroundSky.green()/255.,
						mBackgroundSky.blue()/255.);
		fg::GLRenderer::renderSkySphere(20,64,64,hc,tc,tc);

		glPopAttrib(); // GL_LIGHTING_BIT
		glPopAttrib(); // GL_DEPTH_BUFFER_BIT
		glPopAttrib(); // GL_TEXTURE_BIT
		glPopAttrib(); // GL_ENABLE_BIT

		if (mLighting){
			GLfloat lp[] = {1, 1, 1, 0};
			glLightfv(GL_LIGHT0,GL_POSITION,lp);
			glEnable(GL_LIGHTING);
		}
		else {
			glDisable(GL_LIGHTING);
		}

		float z = std::exp(-mZoom);
		glScalef(z,z,z);

		if (mOrigin) drawOrigin();
		if (mGround) drawGroundPlane();

		if (mUniverse!=NULL){
			foreach(shared_ptr<fg::MeshNode> m, mUniverse->meshNodes()){
				glColor3f(1,1,1);
				// std::cout << m << "\n" << *m << "\n\n";

				m->mesh()->sync(); // make sure normals are okay
				// Subdivide the viewing mesh if required

				shared_ptr<fg::Mesh> old = shared_ptr<fg::Mesh>();
				if (mNumberSubdivs>0){
					// copy the mesh
					old = m->mesh();
					shared_ptr<fg::Mesh> clone = old->clone();
					clone->smoothSubdivide(mNumberSubdivs);
					m->setMesh(clone);
				}

				if (mShadersAvailable and mPhongShader!=NULL and mMeshMode==MM_PHONG){
					mPhongShader->bind();
					mPhongShader->setUniformValue("shininess",(GLfloat)25);

//#ifdef _WIN32
					mPhongShader->setUniformValue("useVertexColour",(GLint)GL_FALSE);
					if (mColourMode==fg::GLRenderer::COLOUR_VERTEX){
					 	mPhongShader->setUniformValue("useVertexColour",(GLint)GL_TRUE);
					}
//#endif

				}

				fg::GLRenderer::RenderMeshMode rmm;
				switch (mMeshMode){
					case MM_SMOOTH: case MM_PHONG: rmm = fg::GLRenderer::RENDER_SMOOTH; break;
					case MM_FLAT: rmm = fg::GLRenderer::RENDER_FLAT; break;
					case MM_WIRE: rmm = fg::GLRenderer::RENDER_WIRE; break;
					case MM_POINTS: rmm = fg::GLRenderer::RENDER_VERTICES; break;
					case MM_TEXTURED: rmm = fg::GLRenderer::RENDER_TEXTURED; break;
				}

				fg::GLRenderer::renderMeshNode(m,rmm,mColourMode); // fg::GLRenderer::RenderMeshMode(DRAW_MODE));

				if (mShadersAvailable and mPhongShader!=NULL and mMeshMode==MM_PHONG){
					mPhongShader->release();
				}

				if (mShadersAvailable and mShowOverWire){
					if (mMeshMode==MM_FLAT or mMeshMode==MM_PHONG or mMeshMode==MM_SMOOTH or mMeshMode==MM_TEXTURED){

						mOverWireShader->bind();
						glColor3f(.1,.1,.1);
						fg::GLRenderer::renderMeshNode(m,fg::GLRenderer::RENDER_WIRE,fg::GLRenderer::COLOUR_NONE); // fg::GLRenderer::RenderMeshMode(DRAW_MODE));
						mOverWireShader->release();
					}
				}

				if (mNumberSubdivs>0){
					m->setMesh(old);
				}

				// clone should get garbage collected...
			}



			if (mShadersAvailable and mPhongShader!=NULL){
				mPhongShader->release();
			}

			if (mShowNodeAxes){
				foreach(shared_ptr<fg::Node> n, mUniverse->nodes()){
					glPushMatrix();
					fg::Mat4 t = n->getCompoundTransform().transpose();
					glMultMatrixd(t.V());
					fg::GLRenderer::renderAxes();
					glPopMatrix();
				}
			}
		}

	}
	glPopMatrix();

#ifdef ENABLE_SSAO
	if (mShadersAvailable and mSSAO and mFBO){
		GLuint glFBO = mFBO->handle();
		CHECK_FOR_GL_ERROR();
		GLint result;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mDepthTex);
		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,0,0,width(),height(),0);

		CHECK_FOR_GL_ERROR();
		GLuint fboTex = mFBO->texture();
		CHECK_FOR_GL_ERROR();
		mFBO->release();


		CHECK_FOR_GL_ERROR();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,width(),0,height());
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		//mAOShader->bind();
		CHECK_FOR_GL_ERROR();

		if (not mAOShader->bind()){
			std::cerr << "Couldn't bind AO shader\n";
		}

		int uniformTex0 = mAOShader->uniformLocation("texture0");
		int uniformTex1 = mAOShader->uniformLocation("texture1");
		int uniformCR = mAOShader->uniformLocation("camerarange");
		int uniformSS = mAOShader->uniformLocation("screensize");

		CHECK_FOR_GL_ERROR();

		if (uniformTex0==-1) std::cerr << "Couldn't find texture0\n";
		else mAOShader->setUniformValue(uniformTex0, (GLuint)0);
		if (uniformTex1==-1) std::cerr << "Couldn't find texture1\n";
		else mAOShader->setUniformValue(uniformTex1, (GLuint)1);
		if (uniformCR==-1) std::cerr << "Couldn't find camerarange\n";
		else mAOShader->setUniformValue(uniformCR,0.1,100);
		if (uniformSS==-1) std::cerr << "Couldn't find screensize\n";
		else mAOShader->setUniformValue(uniformSS,width(),height());

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, mDepthTex);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, fboTex);

		CHECK_FOR_GL_ERROR();

		glPushMatrix();
		// glTranslatef(width()/2,0,0);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(0,0);
		glTexCoord2f(1,0);
		glVertex2f(width(),0);
		glTexCoord2f(1,1);
		glVertex2f(width(),height());
		glTexCoord2f(0,1);
		glVertex2f(0,height());
		glEnd();
		glPopMatrix();

		mAOShader->release();

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		CHECK_FOR_GL_ERROR();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
#endif
}

void FGView::resizeGL(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0*width/height, 0.1, 100);
	glViewport(0,0,width,height);
	glMatrixMode(GL_MODELVIEW);


	if (mShadersAvailable){

#ifdef ENABLE_SSAO
		if (mFBO){
			delete mFBO;
		}
		mFBO = new QGLFramebufferObject(width,height,QGLFramebufferObject::Depth);
		if (!mFBO->isValid()){
			std::cerr << "Framebuffer invalid\n";
			delete mFBO;
			mFBO = NULL;
		}

		// std::cout << "Resizing depth texture (" << width << "," << height << ")\n";

		// create the new depth texture
		if (glIsTexture(mDepthTex)){
			glDeleteTextures(1,&mDepthTex);
		}

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1,&mDepthTex);
		glBindTexture(GL_TEXTURE_2D, mDepthTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		CHECK_FOR_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
	}
}

void FGView::mousePressEvent(QMouseEvent *event)
{
	mMouseState.isTranslating = false;
	mMouseState.isRotating = false;
	mMouseState.isZooming = false;

	Qt::MouseButton button = event->button();
	Qt::KeyboardModifiers mods = event->modifiers();

	mMouseState.lastX = event->x();
	mMouseState.lastY = event->y();

	if (button==Qt::LeftButton  and
		(mods&Qt::ControlModifier or
		 mods&Qt::ShiftModifier or
		 mods&Qt::AltModifier)){
		// Translation
		mMouseState.isTranslating = true;

	}
	else if (button==Qt::LeftButton){
		// Rotation
		mMouseState.isRotating = true;
	}
	else if (button==Qt::MidButton|| button==Qt::RightButton){
		// Zoom
		mMouseState.isZooming = true;
	}
}

void FGView::mouseReleaseEvent(QMouseEvent *event){
	mMouseState.isTranslating = false;
	mMouseState.isRotating = false;
	mMouseState.isZooming = false;
}

void FGView::mouseMoveEvent(QMouseEvent *event)
{
	int x = event->x();
	int y = event->y();

	if (mMouseState.isRotating){
		float q[4];

		float invWidth = 1.f/width();
		float invHeight = 1.f/height();

		float dLastX = 2 * mMouseState.lastX * invWidth - 1;
		float dLastY = -2 * (mMouseState.lastY * invHeight) + 1;
		float dX = 2*x*invWidth - 1;
		float dY = -2*y*invHeight + 1;

		trackball(q,dLastX,dLastY,dX,dY);

		float oldQ[4] = {mRotationQuat[0],mRotationQuat[1],mRotationQuat[2],mRotationQuat[3]};
		add_quats(q,oldQ,mRotationQuat);

		build_rotmatrix(mRotationMatrix, mRotationQuat);

		mMouseState.lastX = x;
		mMouseState.lastY = y;

		update();
	}
	else if (mMouseState.isZooming) {
		double dy = -1.0*(mMouseState.lastY - y)/height();
		mZoom += dy*5;
		mMouseState.lastY = y;

		update();
	}
	else if (mMouseState.isTranslating) {
		float invWidth = 1.f/width();
		float invHeight = 1.f/height();
		float dLastX = 2 * mMouseState.lastX * invWidth - 1;
		float dLastY = -2 * (mMouseState.lastY * invHeight) + 1;
		float dX = 2*x*invWidth - 1;
		float dY = -2*y*invHeight + 1;

		// update position
		// along x-axis (left-right) in view plane...
		mCameraTranslation[0] += 2*(dX-dLastX) * mRotationMatrix[0][0];
		mCameraTranslation[1] += 2*(dX-dLastX) * mRotationMatrix[1][0];
		mCameraTranslation[2] += 2*(dX-dLastX) * mRotationMatrix[2][0];

		// along y-axis (up-down) in view plane
		mCameraTranslation[0] += 2*(dY-dLastY) * mRotationMatrix[0][1];
		mCameraTranslation[1] += 2*(dY-dLastY) * mRotationMatrix[1][1];
		mCameraTranslation[2] += 2*(dY-dLastY) * mRotationMatrix[2][1];

		mMouseState.lastX = x;
		mMouseState.lastY = y;

		update();
	}
}

void FGView::drawOrigin(){
	glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(2);
	fg::GLRenderer::renderAxes();
	glPopAttrib();
}

void FGView::drawGroundPlane(){
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

void FGView::resetCamera()
{
    mZoom = 0;
    mRotationQuat[0] = 0, mRotationQuat[1] = 0, mRotationQuat[2] = 0, mRotationQuat[3] = 1;
    for (int i = 0; i < 4; ++i)
    {
        mRotationQuat[i] = (i == 3 ? 1 : 0);
        for(int j = 0; j < 4; ++j)
            mRotationMatrix[i][j] = (i == j ? 1 : 0);
    }
    mCameraTranslation[0] = mCameraTranslation[1] = mCameraTranslation[2] = 0;
    update();
}

QGLShaderProgram* FGView::loadShader(const char* vtxFileName, const char* fragFileName){
	QGLShaderProgram* prog = new QGLShaderProgram(context());
	QFile vs(QCoreApplication::applicationDirPath() + "/" + QString(FG_BASE_LOCATION) + "assets/shaders/" + vtxFileName);
	QFile fs(QCoreApplication::applicationDirPath() + "/" + QString(FG_BASE_LOCATION) + "assets/shaders/" + fragFileName);
	if (vs.open(QFile::ReadOnly | QFile::Text)
			and
			fs.open(QFile::ReadOnly | QFile::Text))
	{
		prog->addShaderFromSourceCode(QGLShader::Vertex, vs.readAll());
		prog->addShaderFromSourceCode(QGLShader::Fragment, fs.readAll());
		prog->link();
		std::cout << "Linked shader log: " << prog->log().toStdString();
		return prog;
	}
	else {
		std::cerr << "Couldn't load shader: " << vtxFileName << "/" << fragFileName << "\n";
		return NULL;
	}
}

void FGView::checkForOpenGLError(int line){
	GLenum error = glGetError();
	if (error!=GL_NO_ERROR){
		const GLubyte* str = gluErrorString(error);
		if (str){
			std::cerr << "GL ERROR: " << str << " (line " << line << " in fgview.cpp)\n";
		}
	}
	else {
		// std::cout << "GL is fine at line " << line << " (in fgview.cpp)\n";
	}
}
