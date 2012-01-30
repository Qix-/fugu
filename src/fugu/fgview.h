#ifndef FGVIEW_H
#define FGVIEW_H

#include "fg/universe.h"
#include "fg/glrenderer.h"
#include <QGLWidget>

// TODO: Use Lua as an effect file format?
// ref: http://prideout.net/blog/?p=1

class QGLShaderProgram;

#ifdef ENABLE_SSAO
	class QGLFramebufferObject;
#endif

class FGView : public QGLWidget
{
	Q_OBJECT

public:
	FGView(QWidget *parent = 0);
	~FGView();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	void setUniverse(fg::Universe* u);
	void unsetUniverse();

	QColor getBackgroundHorizonColour() const;
	QColor getBackgroundSkyColour() const;

	inline bool origin(){return mOrigin;}
	inline bool ground(){return mGround;}
	inline bool showNodeAxes(){return mShowNodeAxes;}
	inline bool showOverWire(){return mShowOverWire;}
	inline bool lighting(){return mLighting;}

	inline void setSaveSettings(bool ss){mSaveSettings = ss;}

public slots:
	void resetCamera();

	void toggleOrigin(bool show);
	void toggleGround(bool show);
	void toggleShowNodeAxes(bool show);
	void toggleShowOverWire(bool show);
	void toggleLighting(bool on);
	void toggleSSAO(bool on);

	void setNumberOfSubdivs(int num);
	void setSubdivs0();
	void setSubdivs1();
	void setSubdivs2();
	void setSubdivs3();

	void setDrawSmooth();
	void setDrawFlat();
	void setDrawWire();
	void setDrawPoints();
	void setDrawTextured();
	void setDrawPhong();

	void setColourModeNone();
	void setColourModeVertex();

	void setBackgroundHorizonColour(QColor);
	void setBackgroundSkyColour(QColor);
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

	QGLShaderProgram* loadShader(const char* vtxFileName, const char* fragFileName);
	void checkForOpenGLError(int line);

	// The universe we are currently viewing
	// NULL if none
	fg::Universe* mUniverse;

	/*
	int xRot;
	int yRot;
	int zRot;
	QPoint lastPos;
	*/

	QColor mBackgroundHorizon;
	QColor mBackgroundSky;

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
	bool mLighting;
	bool mSSAO;
	bool mShowOverWire;
	int mNumberSubdivs;
	enum MeshMode { MM_SMOOTH, MM_FLAT, MM_WIRE, MM_POINTS, MM_TEXTURED, MM_PHONG };
	MeshMode mMeshMode;
	fg::GLRenderer::ColourMode mColourMode;

	QGLShaderProgram* mPhongShader;
	QGLShaderProgram* mOverWireShader;
	QGLShaderProgram* mAOShader;

	GLuint mDepthTex;

#ifdef ENABLE_SSAO
	QGLFramebufferObject* mFBO;
#endif

	bool mShadersAvailable;

	// QGLShaderProgram* mSubdivisionShader;

	bool mSaveSettings;
};

#endif
