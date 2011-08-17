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

#include "fg/vertex.h"
#include "fg/mesh.h"
#include "fg/meshimpl.h" // VertexImpl
#include "fg/face.h"

namespace fg {
	VertexProxy::VertexProxy(Mesh* m, VertexImpl* vi):Proxy<VertexImpl>(vi),mMesh(m){}

	VertexProxy::~VertexProxy(){}

	Vec3 VertexProxy::getPos() const {
		return constImpl().P();
	}

	void VertexProxy::setPos(Vec3 v){
		pImpl()->P() = v;
	}

	void VertexProxy::setPos(double x, double y, double z){
		pImpl()->P().X() = x;
		pImpl()->P().Y() = y;
		pImpl()->P().Z() = z;
	}

	Vec3 VertexProxy::getN() const {
		return constImpl().cN();
	}

	Vec3 VertexProxy::getColour() const {
		vcg::Color4b c = constImpl().C();
		Vec3 v(c.X()/255.,c.Y()/255.,c.Z()/255.);
		return v;
	}

	void VertexProxy::setColour(Vec3 k){
		vcg::Color4b c(k.getX()*255,k.getY()*255,k.getZ()*255,255);
		pImpl()->C() = c;
	}

	void VertexProxy::setColour(double r, double g, double b){
		pImpl()->C().X() = r*255.;
		pImpl()->C().Y() = g*255;
		pImpl()->C().Z() = b*255;
	}

	void VertexProxy::setUV(double u, double v){
		pImpl()->T().U() = u;
		pImpl()->T().V() = v;
	}

	shared_ptr<FaceProxy> VertexProxy::getAdjacentFace(){
		return mMesh->_newSP(static_cast<FaceImpl*>(pImpl()->VFp()));
	}

	int VertexProxy::getNumBones() const {
		return constImpl().getNumBones();
	}

} // namespace fg

std::ostream& operator<<(std::ostream& o, const fg::VertexProxy& vp){
	return o << "VertexProxy (Vertex@" << &vp.constImpl() << ")";
}
