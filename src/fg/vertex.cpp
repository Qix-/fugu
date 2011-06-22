#include "fg/vertex.h"
#include "fg/meshimpl.h" // VertexImpl

namespace fg {
	VertexProxy::VertexProxy(VertexImpl* vi):Proxy(vi){}
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
}
