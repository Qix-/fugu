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

#include "fg/meshoperators.h"
#include "fg/meshoperators_vcg.h"
#include "fg/meshimpl.h"

#include <vcg/complex/algorithms/nring.h>

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

	boost::shared_ptr<Mesh::VertexSet> getVerticesAtDistance(Mesh* m, VertexProxy v, int n){
		vcg::tri::Nring<MeshImpl>::clearFlags(m->_impl()); // Probably necessary..
		vcg::tri::Nring<MeshImpl> ring(v.pImpl(),m->_impl());
		ring.expand(n); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
		Mesh::VertexSet* l = new Mesh::VertexSet();
		BOOST_FOREACH(VertexImpl* v, ring.lastV){
			l->push_back(boost::shared_ptr<VertexProxy>(new VertexProxy(m,v)));
		}
		ring.clear();
		return boost::shared_ptr<Mesh::VertexSet>(l);
	}
}
