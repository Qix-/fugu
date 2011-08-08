/**
 * \file
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
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in 
 *   the LICENSE file.
 * 
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#include "fg/glrenderer.h"

#include <cassert>
#include <stdexcept>

#include "GL/glew.h"

#include <wrap/gl/trimesh.h>

#include "fg/interpolator.h"
#include "fg/carriercurve.h"
#include "fg/meshimpl.h"
#include "fg/mat4.h"
#include "fg/ppm.h"

namespace fg {

	class MyGLRenderer: public vcg::GlTrimesh<fg::MeshImpl> {
		public:
		MyGLRenderer():vcg::GlTrimesh<fg::MeshImpl>(){}
	};

	void GLRenderer::renderMesh(Mesh* m, RenderMeshMode rmm){
		// vcg::GlTrimesh<fg::MeshImpl> tm;
		MyGLRenderer tm;
		tm.m = m->_impl();
		tm.Update();

		switch (rmm){
			case RENDER_FLAT: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerVert, vcg::GLW::TMNone> (); break;
			case RENDER_SMOOTH: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMNone> ();	break;
			case RENDER_WIRE: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerVert,vcg::GLW::TMNone> (); break;
			case RENDER_VERTICES: // tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break; 
			{
				tm.DrawPointsBase<vcg::GLW::NMPerVert,vcg::GLW::CMPerVert>();
				break;
			}
			case RENDER_TEXTURED: {
				static bool isTexLoaded = false;
				static GLuint tex = 0;

				if (!isTexLoaded){
					// make sure the texture is loaded...
					Ppm ppm("../assets/UV.ppm");
					if (not ppm.IsValid()){
						throw(std::runtime_error("Can't load ../assets/UV.ppm"));
					}
					else {
						tex = ppm.GetGLTex();
						isTexLoaded = true;
					}

				}

				tm.TMId.push_back(tex);
				tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerVert,vcg::GLW::TMPerVert> ();
				break;
			}
			default: {}
		}
	}

	void GLRenderer::renderMesh(boost::shared_ptr<Mesh> m, RenderMeshMode rmm){
		renderMesh(&*m,rmm);
		
		/*
		// vcg::GlTrimesh<fg::MeshImpl> tm;
		MyGLRenderer tm;
		tm.m = m->_impl();
		tm.Update();

		switch (rmm){
			case RENDER_FLAT: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> (); break;
			case RENDER_SMOOTH: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();	break;
			case RENDER_WIRE: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			case RENDER_VERTICES: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			default: {}
		}
		*/
	}

	void GLRenderer::renderMeshNode(boost::shared_ptr<MeshNode> m, RenderMeshMode rmm){
		glPushMatrix();
		Mat4 t = m->getCompoundTransform().transpose();
		glMultMatrixd(t.V());
		renderMesh(m->mesh(),rmm);
		glPopMatrix();
	}

    void GLRenderer::renderCarrier(const gc::CarrierCurve &c, int n, double time){
		if (c.getInterpolator())
		  renderInterpolator(*c.getInterpolator(), n);

        // Draw a tangent
		time = time * 0.2;
		Vec3 pos = c.getInterpolator()->getPosition(time);
		Vec3 X = c.orient(time)*Vec3(1.,0.,0.);
		Vec3 Y = c.orient(time)*Vec3(0.,1.,0.);
		Vec3 Z = c.orient(time)*Vec3(0.,0.,1.);

		X = pos + X;
		Y = pos + Y;
		Z = pos + Z;

		glBegin(GL_LINES);
		glVertex3d(pos.getX(),pos.getY(),pos.getZ());
		glVertex3d(X.getX(),X.getY(),X.getZ());
		glEnd();
		glBegin(GL_LINES);
		glVertex3d(pos.getX(),pos.getY(),pos.getZ());
		glVertex3d(Y.getX(),Y.getY(),Y.getZ());
		glEnd();
		glBegin(GL_LINES);
		glVertex3d(pos.getX(),pos.getY(),pos.getZ());
		glVertex3d(Z.getX(),Z.getY(),Z.getZ());
		glEnd();
	}

	void GLRenderer::renderInterpolator(const spline::Interpolator<Vec3>& s, int n){
		assert (n>0);

		std::vector<Vec3> arr = s.getApproxVector(n);
		std::vector<Vec3> cp = s.getControlPoints();

		glPushAttrib(GL_CURRENT_BIT);
		glPushAttrib(GL_LIGHTING_BIT);
		glPushAttrib(GL_POINT_BIT);

		glDisable(GL_LIGHTING);
		glPointSize(3);

		// Draw Approx Curve
		glColor3f(1,1,1);
		glBegin(GL_LINES);
		for(int i=0;i<arr.size();i++){
			// draw a line from arr[i] to arr[i+1]
			glVertex3d(arr[i].getX(),arr[i].getY(),arr[i].getZ());
			glVertex3d(arr[i+1].getX(),arr[i+1].getY(),arr[i+1].getZ());
		}
		if (!s.isOpen())
		{
			glVertex3d(arr.back().getX(),arr.back().getY(),arr.back().getZ());
			glVertex3d(arr[0].getX(),arr[0].getY(),arr[0].getZ());
		}
		glEnd();

		glBegin(GL_POINTS);
		for(int i=0;i<arr.size();i++){
			glVertex3d(arr[i].getX(),arr[i].getY(),arr[i].getZ());
		}
		glEnd();

		// Draw Convex Hull
		glColor3f(1,0,1);
		glBegin(GL_LINES);
		for(int i=0;i<s.getNumControlPoints()-1;i++){
			// draw a line from arr[i] to arr[i+1]
			glVertex3d(cp[i].getX(),cp[i].getY(),cp[i].getZ());
			glVertex3d(cp[i+1].getX(),cp[i+1].getY(),cp[i+1].getZ());
		}
		glEnd();

		glBegin(GL_POINTS);
		glColor3f(1,0,1);
		glPointSize(10);
		for(int i=0;i<s.getNumControlPoints();i++){
			glVertex3d(cp[i].getX(),cp[i].getY(),cp[i].getZ());
		}
		glEnd();

        glPointSize(6);
		glBegin(GL_POINTS);
		glColor3f(1,0,1);
		for(int i=0;i<s.getNumControlPoints();i++){
			glVertex3d(s.getPosition(0.).getX(),s.getPosition(0.).getY(),s.getPosition(0.).getZ());
		}
		glEnd();

		glPopAttrib();
		glPopAttrib();
		glPopAttrib();
	}

	void GLRenderer::renderAxes(double L){
		glPushAttrib(GL_CURRENT_BIT);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);

#define V glVertex3f
#define C glColor3f
		glBegin(GL_LINES);
		C(1,0,0); V(0,0,0); V(L,0,0);
		C(0,1,0); V(0,0,0); V(0,L,0);
		C(0,0,1); V(0,0,0); V(0,0,L);
		glEnd(); //GL_LINES
#undef C
#undef V
		glPopAttrib();
		glPopAttrib();
	}

	void GLRenderer::renderBone(Vec3 from, Vec3 to, double r){
		glPushAttrib(GL_CURRENT_BIT);
		glPushAttrib(GL_LINE_BIT);
		glPushAttrib(GL_LIGHTING_BIT);

		glDisable(GL_LIGHTING);
		glLineWidth(5);

		glBegin(GL_LINES);
		glVertex3d(from[0],from[1],from[2]);
		glVertex3d(to[0],to[1],to[2]);
		glEnd();

		glPopAttrib();
		glPopAttrib();
		glPopAttrib();
	}

	void GLRenderer::renderArmature(const Armature& arm, bool alsoRenderInitialPose){
		const float JOINT_RADIUS = .1;
		glColor3f(1,1,1);

		foreach(BoneRef pb, arm.bones()){
			glPushMatrix();
			glMultTransposeMatrixd(pb->getCWSTransform().V());
			// GLRenderer::renderAxes(3*JOINT_RADIUS);
			GLRenderer::glutSolidSphere(JOINT_RADIUS,8,8);

			// render bone
			// have to rotate the z-axis to point in the x-axis
			glRotatef(90,0,1,0);
			GLRenderer::glutSolidCone( JOINT_RADIUS*.5, pb->mLength, 8, 4);
			glPopMatrix();
		}

		if (alsoRenderInitialPose){
			glColor3f(1,0,0);
			foreach(BoneRef pb, arm.bones()){
				glPushMatrix();
				glMultTransposeMatrixd(pb->getIWSTransform().V());
				// GLRenderer::renderAxes(3*JOINT_RADIUS);
				GLRenderer::glutWireSphere(JOINT_RADIUS,8,8);

				// render bone
				// have to rotate the z-axis to point in the x-axis
				glRotatef(90,0,1,0);
				GLRenderer::glutWireCone( JOINT_RADIUS*.5, pb->mLength, 8, 4);
				glPopMatrix();
			}


		}
	}
}
