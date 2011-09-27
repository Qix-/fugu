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
#include "fg/glrenderer_vcg.h" // modified <wrap/gl/trimesh.h>

#include "fg/meshimpl.h"
#include "fg/mat4.h"
#include "fg/ppm.h"
#include "fg/gc/interpolator.h"
#include "fg/gc/carriercurve.h"

class MyGLRenderer: public vcg::GlTrimesh<fg::MeshImpl> {
	public:
	MyGLRenderer():vcg::GlTrimesh<fg::MeshImpl>(){}
};

template <vcg::GLW::DrawMode DM, vcg::GLW::TextureMode TM>
void DrawWrapper(MyGLRenderer& tm, fg::GLRenderer::ColourMode cm){
	switch(cm){
		case fg::GLRenderer::COLOUR_NONE: tm.Draw<DM, vcg::GLW::CMNone, TM>(); break;
		case fg::GLRenderer::COLOUR_VERTEX: tm.Draw<DM, vcg::GLW::CMPerVert, TM>(); break;
		case fg::GLRenderer::COLOUR_FACE_MANIFOLD: tm.Draw<DM, vcg::GLW::CMPerManifoldFace, TM>(); break;
	}
}

// BP: Remove glut defines and errors
#define FGAPIENTRY
#define FREEGLUT_EXIT_IF_NOT_INITIALISED(x) ;
#define fgError(x) ;

namespace fg {

	void GLRenderer::renderMesh(Mesh* m, RenderMeshMode rmm, ColourMode cm){
		// vcg::GlTrimesh<fg::MeshImpl> tm;
		MyGLRenderer tm;
		tm.m = m->_impl();
		tm.Update();

		switch (rmm){
			case RENDER_FLAT: {
				// tm.Draw<vcg::GLW::DMFlat, colorMode, vcg::GLW::TMNone> ();
				DrawWrapper<vcg::GLW::DMFlat, vcg::GLW::TMNone>(tm,cm);
				break;
			}
			case RENDER_SMOOTH: {
				//tm.Draw<vcg::GLW::DMSmooth, colorMode, vcg::GLW::TMNone> ();
				DrawWrapper<vcg::GLW::DMSmooth, vcg::GLW::TMNone>(tm,cm);
				break;
			}
			case RENDER_WIRE: {
				//tm.Draw<vcg::GLW::DMWire,     colorMode,vcg::GLW::TMNone> ();
				DrawWrapper<vcg::GLW::DMWire, vcg::GLW::TMNone>(tm,cm);
				break;
			}
			case RENDER_VERTICES: {
				tm.DrawPointsBase<vcg::GLW::NMPerVert, vcg::GLW::CMPerVert>();
				break;
			}
			case RENDER_TEXTURED: {
				static bool isTexLoaded = false;
				static GLuint tex = 0;

				glPushAttrib(GL_TEXTURE_BIT);
				glEnable(GL_TEXTURE_2D);

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
				//tm.Draw<vcg::GLW::DMSmooth, colorMode,vcg::GLW::TMPerVert> ();
				DrawWrapper<vcg::GLW::DMSmooth, vcg::GLW::TMPerVert>(tm,cm);

				glPopAttrib();
				break;
			}
			default: {}
		}
	}

	void GLRenderer::renderMesh(boost::shared_ptr<Mesh> m, RenderMeshMode rmm, ColourMode cm){
		renderMesh(&*m,rmm,cm);
	}

	void GLRenderer::renderMeshNode(boost::shared_ptr<MeshNode> m, RenderMeshMode rmm, ColourMode cm){
		glPushMatrix();
		Mat4 t = m->getCompoundTransform().transpose();
		glMultMatrixd(t.V());
		renderMesh(m->mesh(),rmm,cm);
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

	void GLRenderer::renderInterpolator(const gc::Interpolator<Vec3>& s, int n){
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

	/*
	 * Draws a solid sphere
	 */
	void GLRenderer::renderSkySphere(GLdouble radius, GLint slices, GLint stacks, Vec3 bc, Vec3 hc, Vec3 tc)
	{
		glPushMatrix();
		glRotatef(-90,1,0,0);

	    int i,j;

	    /* Adjust z and radius as stacks are drawn. */

	    double z0,z1;
	    double r0,r1;

	    /* Pre-computed circle */

	    double *sint1,*cost1;
	    double *sint2,*cost2;

	    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSolidSphere" );

	    fghCircleTable(&sint1,&cost1,-slices);
	    fghCircleTable(&sint2,&cost2,stacks*2);

	    /* The top stack is covered with a triangle fan */

	    z0 = 1.0;
	    z1 = cost2[(stacks>0)?1:0];
	    r0 = 0.0;
	    r1 = sint2[(stacks>0)?1:0];

	    glBegin(GL_TRIANGLE_FAN);

        	glColor3f(tc.X(),tc.Y(),tc.Z());
	        glNormal3d(0,0,1);
	        glVertex3d(0,0,radius);

	        for (j=slices; j>=0; j--)
	        {
	            glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
	            glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
	        }

	    glEnd();

	    /* Cover each stack with a quad strip, except the top and bottom stacks */

	    for( i=1; i<stacks-1; i++ )
	    {
	        z0 = z1; z1 = cost2[i+1];
	        r0 = r1; r1 = sint2[i+1];

	        Vec3 c0 = z0>=0?lerp(hc,tc,z0):lerp(bc,hc,z0+1);
	        Vec3 c1 = z1>=0?lerp(hc,tc,z1):lerp(bc,hc,z1+1);

	        glBegin(GL_QUAD_STRIP);

	            for(j=0; j<=slices; j++)
	            {
	            	glColor3f(c1.X(),c1.Y(),c1.Z());
	                glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
	                glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);

	                glColor3f(c0.X(),c0.Y(),c0.Z());
	                glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
	                glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
	            }

	        glEnd();
	    }

	    /* The bottom stack is covered with a triangle fan */

	    z0 = z1;
	    r0 = r1;

	    glBegin(GL_TRIANGLE_FAN);

	        glNormal3d(0,0,-1);
	        glVertex3d(0,0,-radius);
	        glColor3f(bc.X(),bc.Y(),bc.Z());
	        for (j=0; j<=slices; j++)
	        {
	            glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
	            glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
	        }

	    glEnd();

	    /* Release sin and cos tables */

	    free(sint1);
	    free(cost1);
	    free(sint2);
	    free(cost2);

	    glPopMatrix();
	}
}
