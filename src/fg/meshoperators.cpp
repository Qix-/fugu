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

namespace fg {
	void extrude(Mesh* m, VertexProxy v, double distance){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		Extrude::FPUpdateList fpul = m->_fpl()->getUpdateList();

		VertexImpl* impl = v.pImpl();
		Extrude::extrude(
				// static_cast<Extrude::MyMesh*>(m->impl()),
				m->_impl(),
				// static_cast<Extrude::Vertex*&>(),
				impl,
				vpul,
				fpul,
				1,
				static_cast<vcg::Point3d>(v.pImpl()->N()),
				distance);
		//,0.);
	}

	void extrude(Mesh* m, VertexProxy v, int width, Vec3 direction, double length, double expand){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		Extrude::FPUpdateList fpul = m->_fpl()->getUpdateList();
		VertexImpl* impl = v.pImpl();
		Extrude::extrude(
				m->_impl(),
				impl,
				vpul,
				fpul,
				width,
				static_cast<vcg::Point3d>(direction),
				length,
				expand);
	}

	void extrude(Mesh* m, VertexProxy v, int w, Vec3 direction, double magnitude){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		Extrude::FPUpdateList fpul = m->_fpl()->getUpdateList();
		VertexImpl* impl = v.pImpl();
		Extrude::extrude(
				m->_impl(),
				impl,
				vpul,
				fpul,
				w,
				static_cast<vcg::Point3d>(direction),
				magnitude);
	}

	boost::shared_ptr<Mesh::VertexSet> getVerticesAtDistance(Mesh* m, VertexProxy v, int n){
		vcg::tri::Nring<MeshImpl>::clearFlags(m->_impl()); // Probably necessary..
		vcg::tri::Nring<MeshImpl> ring(v.pImpl(),m->_impl());
		ring.expand(n); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
		Mesh::VertexSet* l = new Mesh::VertexSet();
		BOOST_FOREACH(VertexImpl* v, ring.lastV){
			// l->push_back(boost::shared_ptr<VertexProxy>(new VertexProxy(m,v)));
			l->push_back(m->_newSP(v));
		}
		ring.clear();
		return boost::shared_ptr<Mesh::VertexSet>(l);
	}

	boost::shared_ptr<Mesh::VertexSet> getVerticesWithinDistance(Mesh* m, VertexProxy v, int n){
		vcg::tri::Nring<MeshImpl>::clearFlags(m->_impl()); // Probably necessary..
		vcg::tri::Nring<MeshImpl> ring(v.pImpl(),m->_impl());
		ring.expand(n); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
		Mesh::VertexSet* l = new Mesh::VertexSet();
		BOOST_FOREACH(VertexImpl* v, ring.allV){
			// l->push_back(boost::shared_ptr<VertexProxy>(new VertexProxy(m,v)));
			l->push_back(m->_newSP(v));
		}
		ring.clear();
		return boost::shared_ptr<Mesh::VertexSet>(l);
	}

	typedef MeshImpl MyMesh;
	// typedef typename MyMesh::VertexType VertexType;

	typedef MyMesh::VertexType Vertex;
	typedef MyMesh::VertexPointer VertexPointer;
	typedef MyMesh::FaceType Face;
	typedef MyMesh::FacePointer FacePointer;


	boost::shared_ptr<Mesh::VertexSet> nloop(Mesh* mp, VertexProxy vp, int w){
		Vertex* v = vp.pImpl();
		MyMesh* m = mp->_impl();

		// select the n-ring of faces
		vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
		vcg::tri::Nring<MyMesh> ring(v,m);
		ring.expand(w);
		std::vector<FacePointer> internalFaces = ring.allF;
		std::vector<FacePointer> lastF = ring.lastF;
		// std::vector<VertexPointer> edgeLoop = ring.lastV;

		// extract the ordered edgeLoop from the Nring
		std::vector<VertexPointer> lastV = ring.lastV;
		std::vector<VertexPointer> edgeLoop;
		Vertex* v0 = lastV[0];
		edgeLoop.push_back(v0);
		Vertex* oldV = v0;
		std::vector<VertexPointer>::iterator lastVEnd = std::remove(lastV.begin(), lastV.end(), v0);;
		while (lastVEnd!=lastV.begin()){
			// find neighbour of v0 in internal Faces
			bool foundNextItem = false;
			BOOST_FOREACH(FacePointer fp, lastF){
				for(int k=0;k<3;k++){
					if (fp->V(k)==oldV){
						Vertex* nextV = fp->V((k+1)%3);
						std::vector<VertexPointer>::iterator it = std::find(lastV.begin(),lastVEnd,nextV);
						if (it!=lastVEnd){
							// found the next vertex
							edgeLoop.push_back(nextV);
							oldV = nextV;
							foundNextItem = true;
							lastVEnd = std::remove(lastV.begin(), lastVEnd, nextV);
							break;
						}
						else {
							// do nothing
						}
					}
				} // exit break

				if (foundNextItem) break;
			}

			if (foundNextItem==false) {
				// the
				throw("Didn't find the next vertex in the edge loop");
				break;
			}
		}
		// implicitly, the last element should be connected to the first

		// Construct a vertex set from the ring
		Mesh::VertexSet* l = new Mesh::VertexSet();
		BOOST_FOREACH(VertexImpl* v, ring.lastV){
			l->push_back(mp->_newSP(v));
		}

		ring.clear();
		/*
		if (!isEdgeLoop(edgeLoop)){
			throw("Edge loop is either not connected or not closed.");
		}
		*/
		return boost::shared_ptr<Mesh::VertexSet>(l);
	}

	void splitEdge(Mesh* m, Pos p){
		Extrude::VPUpdateList vpul = m->_vpl()->getUpdateList();
		Extrude::FPUpdateList fpul = m->_fpl()->getUpdateList();
		vcg::face::Pos<fg::FaceImpl> vcgpos(p.getF()->pImpl(),p.getE(),p.getV()->pImpl());
		Extrude::splitEdge(m->_impl(),vcgpos,vpul,fpul);
	}
}
