#ifndef FG_POS_H
#define FG_POS_H

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component_ocf.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>
#include <vcg/complex/algorithms/nring.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/color.h>

#include "fg/mesh.h"
#include "fg/meshimpl.h"
#include "fg/util.h"

namespace fg {
	typedef vcg::face::Pos<Face> Pos;

	class PosProxy: public Proxy<Pos> {
	public:
		PosProxy(VertexProxy vp, int edge, FaceProxy fp){}
	};
}

#endif
