#include <QtGui>

#include <GL/glew.h>
#include <QtOpenGL>

#include <cmath>
#include <cstring>

#include "fgview.h"

#include "fg/glrenderer.h"

#include "fge/trackball.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

FGView::FGView(QWidget *parent)
: QGLWidget(QGLFormat(QGL::DepthBuffer), parent)
{
	//logo = 0;
	xRot = 0;
	yRot = 0;
	zRot = 0;

	// set camera defaults
	mZoom = 0;
	float rotq[4] = {0,0,0,1};
	float rotmat[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
	float camtr[3] = {0,0,0};
	memcpy(mRotationQuat,rotq,sizeof rotq);
	memcpy(mRotationMatrix,rotmat,sizeof rotmat);
	memcpy(mCameraTranslation,camtr,sizeof camtr);

	// mouse state
	mMouseState.lastX = 0;
	mMouseState.lastY = 0;
	mMouseState.isRotating = false;
	mMouseState.isTranslating = false;
	mMouseState.isZooming = false;

	// view mode
	mOrigin = true;
	mGround = true;
	mShowNodeAxes = true;
	mEnableLighting = true;
	mNumberSubdivs = 0;
	mMeshMode = MM_FLAT;
	mColourMode = fg::GLRenderer::COLOUR_NONE;

	// theme...

	qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
	qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

FGView::~FGView()
{
}

QSize FGView::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize FGView::sizeHint() const
{
	return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void FGView::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
		emit xRotationChanged(angle);
		updateGL();
	}
}

void FGView::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		emit yRotationChanged(angle);
		updateGL();
	}
}

void FGView::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		emit zRotationChanged(angle);
		updateGL();
	}
}

void FGView::initializeGL()
{
	qglClearColor(qtPurple.dark());

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

void FGView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0,0,4,   0,0,0,   0,1,0);
	glPushMatrix();
	{
		if (mEnableLighting){
			GLfloat lp[] = {1, 1, 1, 0};
			glLightfv(GL_LIGHT0,GL_POSITION,lp);
			glEnable(GL_LIGHTING);
		}
		else {
			glDisable(GL_LIGHTING);
		}

		glMultMatrixf((GLfloat*) mRotationMatrix);
		glTranslatef(mCameraTranslation[0],
				mCameraTranslation[1],
				mCameraTranslation[2]);

		float z = std::exp(-mZoom);
		glScalef(z,z,z);

		if (mOrigin) drawOrigin();
		if (mGround) drawGroundPlane();

		if (mUniverse!=NULL){
			foreach(shared_ptr<fg::MeshNode> m, mUniverse->meshNodes()){
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

				fg::GLRenderer::RenderMeshMode rmm;
				switch (mMeshMode){
					case MM_SMOOTH: rmm = fg::GLRenderer::RENDER_SMOOTH; break;
					case MM_FLAT: rmm = fg::GLRenderer::RENDER_FLAT; break;
					case MM_WIRE: rmm = fg::GLRenderer::RENDER_WIRE; break;
					case MM_POINTS: rmm = fg::GLRenderer::RENDER_VERTICES; break;
					case MM_TEXTURED: rmm = fg::GLRenderer::RENDER_TEXTURED; break;
				}
				fg::GLRenderer::renderMeshNode(m,rmm,mColourMode); // fg::GLRenderer::RenderMeshMode(DRAW_MODE));

				if (mNumberSubdivs>0){
					m->setMesh(old);
				}

				// clone should get garbage collected...
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

	/*
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(xRot + 8 * dy);
		setYRotation(yRot + 8 * dx);
	} else if (event->buttons() & Qt::RightButton) {
		setXRotation(xRot + 8 * dy);
		setZRotation(zRot + 8 * dx);
	}
	lastPos = event->pos();
	*/

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

	// else
	/*
	if (not TwEventMousePosGLFW(x,y)){
		// ...
	}
	*/
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
}
