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

#ifndef FG_FACE_H
#define FG_FACE_H

#include <vector>
#include <list>

#include "fg/util.h"
#include "fg/proxy.h"
#include "fg/vec3.h"

namespace fg {
	// forward decl
	class FaceImpl;
	class Mesh;
	class VertexProxy;

	/**
	 * \brief Provides a proxy to a fg::Face
	 *
	 * A FaceProxy has a number of purposes:
	 * - Provides access to the underlying vcg::Face implementation
	 * - Adds helper methods for the lua bindings
	 *
	 * TODO: Add helper methods for accessing custom data
	 */
	class FaceProxy: public Proxy<FaceImpl> {
	public:
		FaceProxy(Mesh* m, FaceImpl* vi);
		~FaceProxy();

		shared_ptr<VertexProxy> getV(int i) const; ///< Return vertex i, where i is 0, 1 or 2.

		Vec3 getN() const; // hmm, can't be const (because of vcg...)
		void calculateNormal(); // recalculate normal

		bool operator==(const FaceProxy& fp) const;

		Mesh* _mesh() const {return mMesh;}

	private:
		Mesh* mMesh;
	};

	typedef ProxyList<FaceImpl> FaceProxyList;
}

std::ostream& operator<<(std::ostream&, const fg::FaceProxy&);

#endif
