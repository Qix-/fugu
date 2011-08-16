/**
 * \file
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

#include "fg/face.h"
#include "fg/vertex.h"
#include "fg/meshimpl.h" // FaceImpl
#include "fg/mesh.h"

namespace fg {
	FaceProxy::FaceProxy(Mesh* m, FaceImpl* fi):Proxy<FaceImpl>(fi),mMesh(m){}
	FaceProxy::~FaceProxy(){}

	shared_ptr<VertexProxy> FaceProxy::getV(int i) const{
		return mMesh->_newSP(static_cast<VertexImpl*>(constImpl().cV(i)));
	}

	Vec3 FaceProxy::getN() const {
		return constImpl().cN();
	}
} // namespace fg

std::ostream& operator<<(std::ostream& o, const fg::FaceProxy& vp){
	return o << "FaceProxy (Face@" << &vp.constImpl() << ")";
}
