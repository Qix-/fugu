#ifndef FGVIEW_H
#define FGVIEW_H

#include "fg/universe.h"
#include "fg/glrenderer.h"
#include <QGLWidget>

class QGLShaderProgram;

class FGView : public QGLWidget
{
	Q_OBJECT

public:
	FGView(QWidget *parent = 0);
	~FGView();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	void setUniverse(fg::Universe* u){mUniverse = u;}
	void unsetUniverse(){mUniverse = NULL;}

public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	void resetCamera();

	void toggleOrigin(bool show);
	void toggleGround(bool show);
	void toggleShowNodeAxes(bool show);
	void toggleLighting(bool on);
	void setNumberOfSubdivs(int num);

	void setDrawSmooth();
	void setDrawFlat();
	void setDrawWire();
	void setDrawPoints();
	void setDrawTextured();
	void setDrawPhong();

signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void drawOrigin();
	void drawGroundPlane();

private:
	// The universe we are currently viewing
	// NULL if none
	fg::Universe* mUniverse;

	int xRot;
	int yRot;
	int zRot;
	QPoint lastPos;

	QColor mBackgroundColor;

	// Camera information
	float mZoom;
	float mRotationQuat[4];
	float mRotationMatrix[4][4];
	float mCameraTranslation[3];

	struct MouseState {
		//bool leftButtonDown;
		//bool middleButtonDown;
		int lastX;
		int lastY;

		bool isRotating;
		bool isTranslating;
		bool isZooming;
	} mMouseState;

	bool mOrigin;
	bool mGround;
	bool mShowNodeAxes; // show node axes
	bool mEnableLighting;
	int mNumberSubdivs;
	enum MeshMode { MM_SMOOTH, MM_FLAT, MM_WIRE, MM_POINTS, MM_TEXTURED, MM_PHONG };
	MeshMode mMeshMode;
	fg::GLRenderer::ColourMode mColourMode;

	QGLShaderProgram* mPhongShader;
};

#endif
