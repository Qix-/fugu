#include "fg/meshoperators.h"
#include "fg/meshoperators_vcg.h"
#include "fg/meshimpl.h"

namespace fg {
	void extrude(Mesh* m, VertexProxy v, double distance){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		VertexImpl* impl = v.pImpl();
		Extrude::extrude(
				// static_cast<Extrude::MyMesh*>(m->impl()),
				m->_impl(),
				// static_cast<Extrude::Vertex*&>(),
				impl,
				vpul,
				1,
				static_cast<vcg::Point3d>(v.pImpl()->N()),
				distance,
				0.);
	}

	void extrude(Mesh* m, VertexProxy v, int width, Vec3 direction, double length, double expand){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		VertexImpl* impl = v.pImpl();
		Extrude::extrude(
				m->_impl(),
				impl,
				vpul,
				width,
				static_cast<vcg::Point3d>(direction),
				length,
				expand);
	}
}
