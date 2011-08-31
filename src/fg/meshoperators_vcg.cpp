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

#include "fg/meshoperators_vcg.h"
#include "fg/util.h"

#include <list>
#include <set>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>

// #include <boost/foreach.hpp> // in fg/util.h
// #include <boost/tuple/tuple.hpp> // in fg/util.h

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
	void Extrude::error(const char* msg){
		std::cerr << "Extrude: " << msg;
		exit(-1);
	}

	std::set<Extrude::VertexPointer> Extrude::extrude(MyMesh* m, Vertex*& v, VPUpdateList& vpul, FPUpdateList& fpul, int width, vcg::Point3d direction, double length, double expand){
		assert (width >= 1);

		vcg::Point3d center = v->P();
		// select the one-ring of faces
		vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
		vcg::tri::Nring<MyMesh> ring(v,m);
		ring.expand(width); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
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

		// clear ring here, as its internal refs will get screwed up when
		// we add verts later on..
		ring.clear();

		if (!isEdgeLoop(edgeLoop)){
			throw("Edge loop is either not connected or not closed.");
		}

		// Duplicate vertices of edge ring
		// (v' corresponding to v in the edge ring)
		// insideedgeLoop[i] = edgeLoop[i]'
		std::vector<Vertex*> insideEdgeLoop;
		int newVertsStartIndex = m->vert.size();

		// Add new vertices, but note that we need to
		// update the pointers to v and each edgeLoop vertex
		std::vector<VertexPointer*> vtxPtrs;
		BOOST_FOREACH(VertexPointer& ev, edgeLoop){
			vtxPtrs.push_back(&ev);
		}
		vtxPtrs.push_back(&v);
		BOOST_FOREACH(VertexPointer*& ev, vpul){
			vtxPtrs.push_back(ev);
		}

		vcg::tri::Allocator<MyMesh>::AddVertices(*m,edgeLoop.size(),vtxPtrs);

		int di = 0;
		BOOST_FOREACH(Vertex* ev, edgeLoop){
			Vertex* nv = &m->vert[newVertsStartIndex + di];
			insideEdgeLoop.push_back(nv);
			nv->P() = ev->P(); //  + direction*amount; // moved later..
			di++;
		}

		// For all v in edgeLoop
		// for all f attached to v
		// If f is an inside face, then reattach it to v'

		// The reattachments we need to perform
		// (f,i,v) set f->V(i)=v
		typedef boost::tuple<Face*,int,Vertex*> FivTuple;
		std::list<FivTuple> faceAdjustments;

		for(int i=0;i<edgeLoop.size();i++){
			Vertex* ve = edgeLoop[i];
			Vertex* vm1 = edgeLoop[(i-1+edgeLoop.size())%edgeLoop.size()];
			Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
			Vertex* vd = insideEdgeLoop[i];

			// How to find inside faces?
			// 1. first find the face that contains ve,vp1
			Face* startFace = NULL;
			int startIndex = 0; // The index of ve in startFace

			//std::cerr << "ve: " << ve << "\n";

			vcg::face::VFIterator<Face> vfi(ve); //initialize the iterator to the first face
			for(;!vfi.End();++vfi)
			{
				Face* f = vfi.F();
				// std::cerr << "f: " << f << "\n";

				int k=0;
				for(;k<3;k++){
					if (f->V(k)==ve && f->V((k+1)%3)==vp1){
						startFace = f;
						startIndex = k;
						break;
					}
				}
				if (k==3) continue;
				else {
					// we have found the start face
					break;
				}
			}

			if (startFace==NULL){
				// An error has occurred
				// The edge ring is not connected properly
				// TODO: Throw an informative error message
				throw("edge loop probably not fully connected or closed");
			}

			// 2. Next, iterate from this face, until we find the endFace
			// the endFace contains vm1,ve in order
			int cycle = 0; // track how many iterations we do and stop if too many
			vcg::face::Pos<Face> p(startFace,(startIndex+2)%3,ve);
			do {
				// std::cerr << ".\n";
				// Find index of ve in p.f
				int index = 0;
				for(;index<3;index++)
				{ if (p.f->V(index)==ve) break; }
				assert(index!=3);
				faceAdjustments.push_back(boost::make_tuple(p.f,index,vd));

				// iterate to next face, stopping when we cross the edgering
				// i.e., edge p.E() joins ve and vm1
				// as p.E() always starts at ve, we just need to check vm1
				p.FlipF();
				if (p.f->V((p.E()+1)%3)==vm1) break;
				// else
				p.FlipE();

				cycle++;
			} while (cycle < 1000); // shouldnt have more than 1000 neighbour faces!
			if (cycle>=1000){
				throw("Either your vertices have a really high degree (>1000), or the extrusion algorithm might be caught in an infinite loop around a vertex! Check your edge-ring or topology correctness.");
			}
		}

		BOOST_FOREACH(FivTuple& a, faceAdjustments){
			Face* f = a.get<0>(); // this form doesnt seem to work
			// Face* f = FivTuple::get<0>(a);
			int i = a.get<1>(); // FivTuple::get<1>(a);
			Vertex* v = a.get<2>(); // FivTuple::get<2>(a);
			f->V(i) = v;
		}

		// Finally, for all pairs of edges, create two new triangles
		// between them
		int newFacesStartIndex = m->face.size();

		std::vector<FacePointer*> facePointersToUpdate;
		BOOST_FOREACH(FacePointer& f, internalFaces){
			facePointersToUpdate.push_back(&f);
		}
		BOOST_FOREACH(FacePointer*& fv, fpul){
			facePointersToUpdate.push_back(fv);
		}
		vcg::tri::Allocator<MyMesh>::AddFaces(*m,edgeLoop.size()*2,facePointersToUpdate);
		for(int i=0;i<edgeLoop.size();i++){
			Vertex* ve = edgeLoop[i];
			Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
			Vertex* vd = insideEdgeLoop[i];
			Vertex* vdp1 = insideEdgeLoop[(i+1)%insideEdgeLoop.size()];

			Face* f1 = &m->face[newFacesStartIndex+2*i];
			Face* f2 = &m->face[newFacesStartIndex+2*i+1];
			f1->V(0) = ve;
			f1->V(1) = vp1;
			f1->V(2) = vd;
			f2->V(0) = vd;
			f2->V(1) = vp1;
			f2->V(2) = vdp1;
		}

		// finally move all the internal faces
		// NB: make sure each vertex is only moved once
		std::set<VertexPointer> internalVerts;
		BOOST_FOREACH(FacePointer f, internalFaces){
			internalVerts.insert(f->V(0));
			internalVerts.insert(f->V(1));
			internalVerts.insert(f->V(2));
			// f->C() = vcg::Color4b::Red;
		}

		// Shift each vertex
		// And make sure they are aligned perpendicular to the extrusion direction
		double ddotd = direction.dot(direction);
		double vdotd = v->P().dot(direction);
		vcg::Point3d newCenter = center + direction*length;

		BOOST_FOREACH(VertexPointer p, internalVerts){

			if (p==v){
				p->P() = newCenter; // direction*2*length;
			}
			else {
				p->P() += direction*(length + (vdotd-p->P().dot(direction))/ddotd);

				// plus the expansion
				vcg::Point3d exp = (newCenter-p->P())*-expand;
				p->P() += exp;
			}
		}


		// Do a big ass topo update
		// TODO: can efficiencize this by manually fixing ptrs etc above
		vcg::tri::UpdateTopology<MyMesh>::FaceFace(*m);
		vcg::tri::UpdateTopology<MyMesh>::VertexFace(*m);

		return internalVerts;
	}

	std::set<Extrude::VertexPointer> Extrude::extrude(MyMesh* m, Vertex*& v, VPUpdateList& vpul, FPUpdateList& fpul, int width, vcg::Point3d direction, double length)
	{
		assert (width >= 1);

		vcg::Point3d center = v->P();
		// select the one-ring of faces
		vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
		vcg::tri::Nring<MyMesh> ring(v,m);
		ring.expand(width); // NB: expand(2 or more) will not give the verts in proper sequence, so have to order them properly
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

		// clear ring here, as its internal refs will get screwed up when
		// we add verts later on..
		ring.clear();

		if (!isEdgeLoop(edgeLoop)){
			throw("Edge loop is either not connected or not closed.");
		}

		// Duplicate vertices of edge ring
		// (v' corresponding to v in the edge ring)
		// insideedgeLoop[i] = edgeLoop[i]'
		std::vector<Vertex*> insideEdgeLoop;
		int newVertsStartIndex = m->vert.size();

		// Add new vertices, but note that we need to
		// update the pointers to v and each edgeLoop vertex
		std::vector<VertexPointer*> vtxPtrs;
		BOOST_FOREACH(VertexPointer& ev, edgeLoop){
			vtxPtrs.push_back(&ev);
		}
		vtxPtrs.push_back(&v);
		BOOST_FOREACH(VertexPointer*& ev, vpul){
			vtxPtrs.push_back(ev);
		}

		vcg::tri::Allocator<MyMesh>::AddVertices(*m,edgeLoop.size(),vtxPtrs);

		int di = 0;
		BOOST_FOREACH(Vertex* ev, edgeLoop){
			Vertex* nv = &m->vert[newVertsStartIndex + di];
			insideEdgeLoop.push_back(nv);
			nv->P() = ev->P(); //  + direction*amount; // moved later..
			di++;
		}

		// For all v in edgeLoop
		// for all f attached to v
		// If f is an inside face, then reattach it to v'

		// The reattachments we need to perform
		// (f,i,v) set f->V(i)=v
		typedef boost::tuple<Face*,int,Vertex*> FivTuple;
		std::list<FivTuple> faceAdjustments;

		for(int i=0;i<edgeLoop.size();i++){
			Vertex* ve = edgeLoop[i];
			Vertex* vm1 = edgeLoop[(i-1+edgeLoop.size())%edgeLoop.size()];
			Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
			Vertex* vd = insideEdgeLoop[i];

			// How to find inside faces?
			// 1. first find the face that contains ve,vp1
			Face* startFace = NULL;
			int startIndex = 0; // The index of ve in startFace

			//std::cerr << "ve: " << ve << "\n";

			vcg::face::VFIterator<Face> vfi(ve); //initialize the iterator to the first face
			for(;!vfi.End();++vfi)
			{
				Face* f = vfi.F();
				// std::cerr << "f: " << f << "\n";

				int k=0;
				for(;k<3;k++){
					if (f->V(k)==ve && f->V((k+1)%3)==vp1){
						startFace = f;
						startIndex = k;
						break;
					}
				}
				if (k==3) continue;
				else {
					// we have found the start face
					break;
				}
			}

			if (startFace==NULL){
				// An error has occurred
				// The edge ring is not connected properly
				// TODO: Throw an informative error message
				throw("edge loop probably not fully connected or closed");
			}

			// 2. Next, iterate from this face, until we find the endFace
			// the endFace contains vm1,ve in order
			int cycle = 0; // track how many iterations we do and stop if too many
			vcg::face::Pos<Face> p(startFace,(startIndex+2)%3,ve);
			do {
				// std::cerr << ".\n";
				// Find index of ve in p.f
				int index = 0;
				for(;index<3;index++)
				{ if (p.f->V(index)==ve) break; }
				assert(index!=3);
				faceAdjustments.push_back(boost::make_tuple(p.f,index,vd));

				// iterate to next face, stopping when we cross the edgering
				// i.e., edge p.E() joins ve and vm1
				// as p.E() always starts at ve, we just need to check vm1
				p.FlipF();
				if (p.f->V((p.E()+1)%3)==vm1) break;
				// else
				p.FlipE();

				cycle++;
			} while (cycle < 1000); // shouldnt have more than 1000 neighbour faces!
			if (cycle>=1000){
				throw("Either your vertices have a really high degree (>1000), or the extrusion algorithm might be caught in an infinite loop around a vertex! Check your edge-ring or topology correctness.");
			}
		}

		BOOST_FOREACH(FivTuple& a, faceAdjustments){
			Face* f = a.get<0>(); // this form doesnt seem to work
			// Face* f = FivTuple::get<0>(a);
			int i = a.get<1>(); // FivTuple::get<1>(a);
			Vertex* v = a.get<2>(); // FivTuple::get<2>(a);
			f->V(i) = v;
		}

		// Finally, for all pairs of edges, create two new triangles
		// between them
		int newFacesStartIndex = m->face.size();

		std::vector<FacePointer*> facePointersToUpdate;
		BOOST_FOREACH(FacePointer& f, internalFaces){
			facePointersToUpdate.push_back(&f);
		}
		BOOST_FOREACH(FacePointer*& fv, fpul){
			facePointersToUpdate.push_back(fv);
		}
		vcg::tri::Allocator<MyMesh>::AddFaces(*m,edgeLoop.size()*2,facePointersToUpdate);
		for(int i=0;i<edgeLoop.size();i++){
			Vertex* ve = edgeLoop[i];
			Vertex* vp1 = edgeLoop[(i+1)%edgeLoop.size()];
			Vertex* vd = insideEdgeLoop[i];
			Vertex* vdp1 = insideEdgeLoop[(i+1)%insideEdgeLoop.size()];

			Face* f1 = &m->face[newFacesStartIndex+2*i];
			Face* f2 = &m->face[newFacesStartIndex+2*i+1];
			f1->V(0) = ve;
			f1->V(1) = vp1;
			f1->V(2) = vd;
			f2->V(0) = vd;
			f2->V(1) = vp1;
			f2->V(2) = vdp1;
		}

		// finally move all the internal faces
		// NB: make sure each vertex is only moved once
		std::set<VertexPointer> internalVerts;
		BOOST_FOREACH(FacePointer f, internalFaces){
			internalVerts.insert(f->V(0));
			internalVerts.insert(f->V(1));
			internalVerts.insert(f->V(2));
			// f->C() = vcg::Color4b::Red;
		}

		// Shift each vertex
		Vec3 diff = direction*length;
		BOOST_FOREACH(VertexPointer p, internalVerts){
			p->P() += diff;
		}

		// Do a big ass topo update
		// TODO: can make efficient by manually fixing ptrs etc above
		vcg::tri::UpdateTopology<MyMesh>::FaceFace(*m);
		vcg::tri::UpdateTopology<MyMesh>::VertexFace(*m);

		// XXX: for debugging only!
		//vcg::tri::UpdateTopology<MyMesh>::TestFaceFace(*m);
		//vcg::tri::UpdateTopology<MyMesh>::TestVertexFace(*m);

		return internalVerts;
	}

	void Extrude::splitEdge(MyMesh* m, Extrude::Pos& pos, VPUpdateList& vpul, FPUpdateList& fpul){
		/*
		 * Given pos = (v,e,f)
		 * find the following elements...
		 *
		 * v4----v3
		 * |f2__/ |
		 * | /  f1|
		 * v1----v2
		 * And the midpoint=(v1+v3)/2
		 */

		VertexPointer v1,v2,v3,v4;
		FacePointer f1, f2;

		v1 = pos.V();
		f1 = pos.F();

		pos.FlipE();
		pos.FlipV();
		v2 = pos.V();
		pos.FlipV();
		pos.FlipE();

		pos.FlipV();
		v3 = pos.V();
		pos.FlipV();

		pos.FlipF();
		f2 = pos.F();
		pos.FlipE();
		pos.FlipV();
		v4 = pos.V();

		assert(v1!=v2 and v1!=v3 and v1!=v4 and v2!=v3 and v2!=v4 and v3!=v4 and f1!=f2 and "splitEdge failed, is edge correctly formed?");

		// NOTE: pos is now useless/invalid

		Vec3 midpoint = (v1->P() + v3->P())/2;

		// Delete f',f, and update any pointers
		vcg::tri::Allocator<MyMesh>::DeleteFace(*m,*f1);
		vcg::tri::Allocator<MyMesh>::DeleteFace(*m,*f2);
		foreach(FacePointer*& fv, fpul){
			if (*fv==f1 or *fv==f2){
				*fv = NULL;
			}
		}

		/*
		 * Add a new vertex, vn, to the midpoint.
		 * And update the all the vertex pointers, including v1-v4
		 */
		VertexPointer vn;

		std::vector<VertexPointer*> vtxPtrs;
		vtxPtrs.push_back(&v1);
		vtxPtrs.push_back(&v2);
		vtxPtrs.push_back(&v3);
		vtxPtrs.push_back(&v4);
		BOOST_FOREACH(VertexPointer*& ev, vpul){
			vtxPtrs.push_back(ev);
		}
		VertexIterator newvi = vcg::tri::Allocator<MyMesh>::AddVertices(*m,1,vtxPtrs);
		vn = &*newvi;
		vn->P() = midpoint;

		// TODO: Interpolate other parameters, e.g., colour and uv?

		// Add the four new faces,
		// with appropriate topological pointers
		// and recomputed normals?
		std::vector<FacePointer*> facePointersToUpdate;
		facePointersToUpdate.push_back(&f1);
		facePointersToUpdate.push_back(&f2);
		BOOST_FOREACH(FacePointer*& fv, fpul){
			facePointersToUpdate.push_back(fv);
		}
		FaceIterator fi = vcg::tri::Allocator<MyMesh>::AddFaces(*m,4,facePointersToUpdate);

		VertexPointer faces[][3] = {
				{v1,v2,vn},
				{v2,v3,vn},
				{v3,v4,vn},
				{v4,v1,vn}
		};

		int index = 0;
		for(;fi!=m->face.end();fi++,index++)
		{
			FacePointer face = &*fi;
			face->V(0) = faces[index][0];
			face->V(1) = faces[index][1];
			face->V(2) = faces[index][2];
		}
		assert(index==4 and "splitEdge failed, four edges not added");

		// Do a big ass topo update
		// TODO: can make efficient by manually fixing ptrs etc above
		vcg::tri::UpdateTopology<MyMesh>::FaceFace(*m);
		vcg::tri::UpdateTopology<MyMesh>::VertexFace(*m);
	}

	/**
	 * Checks if loop is a connected closed edge loop.
	 *
	 * @param loop
	 * @return
	 */
	bool Extrude::isEdgeLoop(std::vector<MyMesh::VertexPointer>& loop){
		//std::cout << "isEdgeLoop: loop.size = " << loop.size() << "\n";

		if (loop.size()<3) return false;
		MyMesh::VertexPointer p = loop[loop.size()-1];
		BOOST_FOREACH(MyMesh::VertexPointer np, loop){
			// p should be connected to np
			bool isPConnectedToNP = false;
			vcg::face::VFIterator<MyMesh::FaceType> vfi(p);
			while(!vfi.End()){
				if (vfi.f->V(0)==np || vfi.f->V(1)==np || vfi.f->V(2)==np)
				{
					isPConnectedToNP = true;
					break;
				}
				++vfi;
			}

			if (!isPConnectedToNP){
				return false;
			}
			p = np;
		}
		return true;
	}
}
