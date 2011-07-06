#ifndef FG_GLRENDERER_H
#define FG_GLRENDERER_H

#include "fg/vec3.h"
#include "fg/mesh.h"
#include "fg/carriercurve.h"

#include <boost/shared_ptr.hpp>

namespace fg {
	// Forward decl
	namespace spline {
		template <typename T> class Interpolator;
	}

	/**
	 * GLRenderer: Renders objects using OpenGL
	 */
	class GLRenderer {
	public:
		enum RenderMeshMode{RENDER_FLAT=0,RENDER_SMOOTH,RENDER_WIRE,RENDER_VERTICES};

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
        static void renderCarrier(const spline::CarrierCurve &c, int n, double time);
	};
}

#endif
