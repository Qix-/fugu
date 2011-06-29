#ifndef FG_VERTEX_H
#define FG_VERTEX_H

#include <vector>
#include <list>

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>

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

		Vec3 getN(); // hmm, can't be const (because of vcg...)

		// Special functions

	};

	/**
	 * A VertexProxyList maintains a list of vertexProxies and is responsible
	 * for updating the internal pointers when the vertices change location or
	 * notifying proxies if vertices are deleted..
	 *
	 * XXX: Need to accommodate VCG's Allocator PointerUpdater design.
	 * 		i.e., be compatible with a std::vector<VertexPointer*>...
	 */
	class VertexProxyList {
	public:
		VertexProxyList();
		~VertexProxyList();

		/**
		 * Add a new vertexproxy to this list. It will be removed when their are no
		 * more references to the vertexproxy (besides this one).		 *
		 */
		void add(boost::shared_ptr<VertexProxy>);

		/**
		 * Remove any unreferenced proxies.
		 *
		 * XXX: Using shared_ptr::use_count may be slow according to boost docs...
		 */
		void doGarbageCollection();

		/**
		 * Return a list of pointers to pointers to vcg::vertex,
		 * this will be used in the vcg Allocator to update the
		 * internal refs of VertexProxies
		 */
		std::vector<VertexImpl**> getUpdateList();

	private:
		std::list<boost::shared_ptr<VertexProxy> > mProxies;
	};

}

std::ostream& operator<<(std::ostream&, const fg::VertexProxy&);

#endif
