#include <QtGui>

#include <GL/glew.h>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QSettings>

#include <cmath>
#include <cstring>

#include "fgview.h"

#include "fg/glrenderer.h"


#include "fg_config.h"

#include "trackball.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

FGView::FGView(QWidget *parent)
: QGLWidget(parent) // NOTE: GLformat set in main.cpp
, mUniverse(NULL) // Very important to intialise as null
, mSaveSettings(true)
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

	// load the shaders
	mPhongShader = new QGLShaderProgram(context());
	QFile phongV(QString(FG_BASE_LOCATION) + "assets/shaders/phong.vert");
	QFile phongF(QString(FG_BASE_LOCATION) + "assets/shaders/phong.frag");
	if (phongV.open(QFile::ReadOnly | QFile::Text)
			and
			phongF.open(QFile::ReadOnly | QFile::Text))
	{
		mPhongShader->addShaderFromSourceCode(QGLShader::Vertex, phongV.readAll());
		mPhongShader->addShaderFromSourceCode(QGLShader::Fragment, phongF.readAll());
		mPhongShader->link();
		// mPhongShader->bind();

		mPhongShader->setUniformValue("shininess",(GLfloat)5);
	}
	else {
		std::cerr << "Couldn't load phong.vert or phong.frag\n";
	}

	mOverWireShader = new QGLShaderProgram(context());
	QFile wireV(QString(FG_BASE_LOCATION) + "assets/shaders/overwire.vert");
	QFile wireF(QString(FG_BASE_LOCATION) + "assets/shaders/passthru.frag");
	if (wireV.open(QFile::ReadOnly | QFile::Text)
			and
			wireF.open(QFile::ReadOnly | QFile::Text))
	{
		mOverWireShader->addShaderFromSourceCode(QGLShader::Vertex, wireV.readAll());
		mOverWireShader->addShaderFromSourceCode(QGLShader::Fragment, wireF.readAll());
		mOverWireShader->link();
		// mPhongShader->bind();
	}
	else {
		std::cerr << "Couldn't load overwire.vert or passthru.frag\n";
	}

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
}

void FGView::paintGL()
{
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

				if (mPhongShader!=NULL and mMeshMode==MM_PHONG){
					mPhongShader->bind();
					mPhongShader->setUniformValue("shininess",(GLfloat)25);

#ifdef _WIN32
					mPhongShader->setUniformValue("useVertexColour",(GLint)GL_FALSE);
					if (mColourMode==fg::GLRenderer::COLOUR_VERTEX){
					 	mPhongShader->setUniformValue("useVertexColour",(GLint)GL_TRUE);
					}
#endif

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

				if (mPhongShader!=NULL and mMeshMode==MM_PHONG){
					mPhongShader->release();
				}

				if (mShowOverWire){
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



			if (mPhongShader!=NULL){
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
}

void FGView::resizeGL(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0*width/height, 0.1, 100);
	glViewport(0,0,width,height);
	glMatrixMode(GL_MODELVIEW);
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
