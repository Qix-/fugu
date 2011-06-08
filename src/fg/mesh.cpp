#include "fg/mesh.h"

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>

#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/refine.h>

#include <vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>

#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>


#include <boost/foreach.hpp>

using namespace vcg;
namespace fg {
	Mesh::Mesh()
	:mMesh()
	{

	}

	Mesh::~Mesh(){
	}

	Mesh::VertexContainer& Mesh::vertices(){
		return mMesh.vert;
	}

	boost::shared_ptr<Mesh::VertexSet> Mesh::selectAllVertices(){
		boost::shared_ptr<VertexSet> r(new VertexSet());
		BOOST_FOREACH(Vertex& v, mMesh.vert){
			r->push_back(&v);
		}
		return r;
	}

	void Mesh::getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz){
		vcg::tri::UpdateBounding<MyMesh>::Box(mMesh);
		minx = mMesh.bbox.min.X();
		miny = mMesh.bbox.min.Y();
		minz = mMesh.bbox.min.Z();
		maxx = mMesh.bbox.max.X();
		maxy = mMesh.bbox.max.Y();
		maxz = mMesh.bbox.max.Z();
	}

	// Modifiers
	void Mesh::subdivide(int levels){
		if (levels <= 0) return;

		// TODO
		/*
		vcg::tri::UpdateTopology<MyMesh>::FaceFace(mMesh);
		vcg::tri::UpdateTopology<MyMesh>::VertexFace(mMesh);

		for(int i=0;i<levels;i++)
			vcg::Refine(mMesh,MidPoint<MyMesh>(&mMesh));

		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(mMesh);
		vcg::tri::UpdateNormals<MyMesh>::PerFaceNormalized(mMesh);
		*/
	}

	void Mesh::drawGL(){
		if (glTriMesh.m == NULL){
			glTriMesh.m = &mMesh;
		}
		vcg::tri::UpdateNormals<MyMesh>::PerFace(mMesh);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexFromCurrentFaceNormal(mMesh);

		glTriMesh.Update();

		glTriMesh.Draw<vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone> ();

		glPushAttrib(GL_LIGHTING_BIT);
		glPushAttrib(GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glColor3f(0,0,0);
		glTriMesh.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
		glPopAttrib();
		glPopAttrib();
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Icosahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Icosahedron<MyMesh>(m->mMesh);
		return boost::shared_ptr<Mesh>(m);
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Mesh& mesh){
	return o << "mesh " << &mesh;
}

std::ostream& operator<<(std::ostream& o, const fg::Mesh::Vertex& v){
	return o << "vertex " << &v;
}
