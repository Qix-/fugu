#ifndef FG_VERTEX_H
#define FG_VERTEX_H

#include "fg/proxy.h"
#include "fg/vec3.h"

namespace fg {
	/// Forward declaration of vertex implementation
	class VertexImpl;

	/**
	 * A VertexProxy has a number of purposes:
	 * - Provides access to the underlying vcg::Vertex implementation
	 * - Adds helper methods for the lua bindings
	 * - TODO: Add helper methods for accessing custom data
	 *
	 */
	class VertexProxy: public Proxy<VertexImpl> {
	public:
		VertexProxy(VertexImpl* vi);
		~VertexProxy();

		// Accessors
		Vec3 getPos() const;
		void setPos(Vec3 v);
		void setPos(double x, double y, double z);
	};
}

std::ostream& operator<<(std::ostream&, const fg::VertexProxy&);

#endif
