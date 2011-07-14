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

#include "fg/meshimpl.h"

#include <vcg/complex/allocate.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>

namespace fg {
	void _copyFloatMeshIntoMesh(_FloatMeshImpl& fm, MeshImpl& m){
		vcg::tri::Allocator<MeshImpl>::AddVertices(m,fm.vert.size());
		std::map<_FloatVertexImpl*,int> ptrMap;
		for(int i=0;i<fm.vert.size();i++){
			ptrMap[&fm.vert[i]] = i;
			for(int c=0;c<3;c++){
				m.vert[i].P()[c] = fm.vert[i].P()[c];
				m.vert[i].N()[c] = fm.vert[i].N()[c];
				//m.vert[i].C()[c] = fm.vert[i].C()[c];
			}
		}

		vcg::tri::Allocator<MeshImpl>::AddFaces(m,fm.face.size());
		for(int i=0;i<fm.face.size();i++){
			FaceImpl* f = &m.face[i];
			f->V(0) = &m.vert[ptrMap[fm.face[i].V(0)]];
			f->V(1) = &m.vert[ptrMap[fm.face[i].V(1)]];
			f->V(2) = &m.vert[ptrMap[fm.face[i].V(2)]];
		}

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(m);
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(m);
	}
}
