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
	// forward decl
	class VertexImpl;
	class Mesh;

	/**
	 * \brief Provides a proxy to a fg::Vertex
	 *
	 * A VertexProxy has a number of purposes:
	 * - Provides access to the underlying vcg::Vertex implementation
	 * - Adds helper methods for the lua bindings
	 * 
	 * TODO: Add helper methods for accessing custom data
	 */
	class VertexProxy: public Proxy<VertexImpl> {
	public:
		VertexProxy(Mesh* m, VertexImpl* vi);
		~VertexProxy();

		// Accessors
		Vec3 getPos() const;
		void setPos(Vec3 v);
		void setPos(double x, double y, double z);

		Vec3 getN() const;

		Vec3 getColour() const;
		void setColour(Vec3 c);
		void setColour(double r, double g, double b);

		void setUV(double u, double v);

		// Special functions
		int getNumBones() const;

	private:
		Mesh* mMesh;
	};

	/**
	 * A VertexProxyList maintains a list of vertexProxies and is responsible
	 * for updating the internal pointers when the vertices change location or
	 * notifying proxies if vertices are deleted..
	 *
	 * XXX: Need to accommodate VCG's Allocator PointerUpdater design.
	 * 		i.e., be compatible with a std::vector<VertexPointer*>...
	 */
	typedef ProxyList<VertexImpl> VertexProxyList;

}

std::ostream& operator<<(std::ostream&, const fg::VertexProxy&);

#endif
