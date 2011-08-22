#include "fg/pos.h"

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
	Pos::Pos( shared_ptr<FaceProxy> fp, int edge, shared_ptr<VertexProxy> vp)
	:mPos(fp->pImpl(),edge,vp->pImpl())
	,mMesh(fp->_mesh())
	{

	}

	Pos::Pos( const Pos& p )
	:mPos(p.mPos.f, p.mPos.z, p.mPos.v)
	,mMesh(p.mMesh)
	{


	}

	void Pos::flipV(){
		mPos.FlipV();
	}

	void Pos::flipE(){
		mPos.FlipE();
	}

	void Pos::flipF(){
		mPos.FlipF();
	}

	shared_ptr<VertexProxy> Pos::getV() {
		return mMesh->_newSP(mPos.V());
	}

	int Pos::getE() {
		return mPos.E();
	}

	shared_ptr<FaceProxy> Pos::getF() {
		return mMesh->_newSP(mPos.F());
	}

	bool Pos::operator==(const Pos& p) const{
		return mPos==p.mPos;
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Pos& p){
	return o << "Pos@" << &p;
}
