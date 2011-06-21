#include "fg/meshoperators.h"
#include "fg/meshoperators_vcg.h"
#include "fg/meshimpl.h"

namespace fg {
	void extrude(Mesh* m, VertexProxy v, double distance){
		Extrude::VPUpdateList vpul;
		VertexImpl* impl = v.impl();
		Extrude::extrude(
				// static_cast<Extrude::MyMesh*>(m->impl()),
				m->impl(),
				// static_cast<Extrude::Vertex*&>(),
				impl,
				vpul,
				1,
				static_cast<vcg::Point3d>(v.impl()->N()),
				distance,
				0.);
	}
}
