#include "fg/glrenderer.h"

#include <cassert>
#include "GL/glew.h"

#include <wrap/gl/trimesh.h>

#include "fg/interpolator.h"
#include "fg/carriercurve.h"
#include "fg/meshimpl.h"
#include "fg/mat4.h"

namespace fg {

	void GLRenderer::renderMesh(Mesh* m, RenderMeshMode rmm){
		vcg::GlTrimesh<fg::MeshImpl> tm;
		tm.m = m->_impl();
		tm.Update();

		switch (rmm){
			case RENDER_FLAT: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> (); break;
			case RENDER_SMOOTH: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();	break;
			case RENDER_WIRE: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			case RENDER_VERTICES: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			default: {}
		}
	}

	void GLRenderer::renderMesh(boost::shared_ptr<Mesh> m, RenderMeshMode rmm){
		vcg::GlTrimesh<fg::MeshImpl> tm;
		tm.m = m->_impl();
		tm.Update();

		switch (rmm){
			case RENDER_FLAT: tm.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> (); break;
			case RENDER_SMOOTH: tm.Draw<vcg::GLW::DMSmooth, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();	break;
			case RENDER_WIRE: tm.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			case RENDER_VERTICES: tm.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> (); break;
			default: {}
		}
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
		if (time > 3.f) time = 3.f;
		Vec3 pos = c.getInterpolator()->getPosition(time);
		Vec3 U;

		U = pos + c.orient(time,1.,0.);

		glBegin(GL_LINES);
		glVertex3d(pos.getX(),pos.getY(),pos.getZ());
		glVertex3d(U.getX(),U.getY(),U.getZ());
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
		for(int i=0;i<n;i++){
			// draw a line from arr[i] to arr[i+1]
			glVertex3d(arr[i].getX(),arr[i].getY(),arr[i].getZ());
			glVertex3d(arr[i+1].getX(),arr[i+1].getY(),arr[i+1].getZ());
		}
		glEnd();

		glBegin(GL_POINTS);
		for(int i=0;i<n+1;i++){
			// draw a line from arr[i] to arr[i+1]
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
}
