/**
 * \file
 * \author ben wetterj
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

#ifndef FG_GLRENDERER_H
#define FG_GLRENDERER_H

#include "fg/vec3.h"
#include "fg/mesh.h"
#include "fg/carriercurve.h"
#include "fg/armature.h"

#include <GL/glew.h>
#include <boost/shared_ptr.hpp>

namespace fg {
	// Forward decl
	namespace spline {
		template <typename T> class Interpolator;
	}

	/**
	 * \brief Renders fg objects using OpenGL
	 */
	class GLRenderer {
	public:
		enum RenderMeshMode{RENDER_FLAT=0,RENDER_SMOOTH,RENDER_WIRE,RENDER_VERTICES,RENDER_TEXTURED};

		static void renderMesh(Mesh* m, RenderMeshMode rmm = RENDER_FLAT);
		static void renderMesh(boost::shared_ptr<Mesh> m, RenderMeshMode rmm = RENDER_FLAT);
		static void renderMeshNode(boost::shared_ptr<MeshNode> m, RenderMeshMode rmm = RENDER_FLAT);

		/**
		 * Render an approximation of a curve interpolator
		 *
		 * @param s the curve
		 * @param n the number of segments
		 */
		static void renderInterpolator(const spline::Interpolator<Vec3>& s, int n);
        static void renderCarrier(const gc::CarrierCurve &c, int n, double time);

        // Helpful primitives
        static void renderAxes(double lengthOfAxes = 1);
        static void renderBone(Vec3 from, Vec3 to, double r = 0);
        static void renderArmature(const Armature& arm, bool alsoRenderInitialPose = false);

        // GLUT primitives (implementation in glut_primitives.cpp)
        static void glutWireCube( GLdouble dSize );
        static void glutSolidCube( GLdouble dSize );
        static void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
        static void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
        static void glutSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
        static void glutWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
        static void glutSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks );
        static void glutWireCone( GLdouble base, GLdouble height, GLint slices, GLint stacks);
	};
}

#endif
