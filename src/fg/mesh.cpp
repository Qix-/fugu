#include "fg/mesh.h"
#include "fg/meshimpl.h"

// VCG
#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/topology.h> // NB: Needed this for Refine!

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
	:mpMesh(NULL)
	{
		mpMesh = new MeshImpl();
	}

	Mesh::~Mesh(){
		delete mpMesh;
	}

	/*
	Mesh::VertexContainer& Mesh::vertices(){
		return mMesh.vert;
	}
	*/

	boost::shared_ptr<Mesh::VertexSet> Mesh::selectAllVertices(){
		boost::shared_ptr<VertexSet> r(new VertexSet());
		BOOST_FOREACH(VertexImpl& v, mpMesh->vert){
			// only return non-dead vertices
			if (!v.IsD()){
				r->push_back(VertexProxy(&v));
			}
		}
		return r;
	}

	void Mesh::getBounds(double& minx, double& miny, double& minz, double& maxx, double& maxy, double& maxz){
		vcg::tri::UpdateBounding<MeshImpl>::Box(*mpMesh);
		minx = mpMesh->bbox.min.X();
		miny = mpMesh->bbox.min.Y();
		minz = mpMesh->bbox.min.Z();
		maxx = mpMesh->bbox.max.X();
		maxy = mpMesh->bbox.max.Y();
		maxz = mpMesh->bbox.max.Z();
	}

	// Modifiers
	void Mesh::subdivide(int levels){
		if (levels <= 0) return;

		// TODO

		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*mpMesh);
		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*mpMesh);

		//vcg::face::IsManifold<MeshImpl>;
		//vcg::face::IsBorder<MeshImpl>;

		// vcg::face::IsManifold<MeshImpl::FaceType>(mMesh.face[0], 0);

		for(int i=0;i<levels;i++)
			vcg::Refine(*mpMesh,vcg::MidPoint<MeshImpl>(mpMesh));

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*mpMesh);
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*mpMesh);

		//vcg::tri::UpdateNormals<MeshImpl>::PerVertexNormalizedPerFace(mMesh);
		//vcg::tri::UpdateNormals<MeshImpl>::PerFaceNormalized(mMesh);
	}

	void Mesh::drawGL(){
		static vcg::GlTrimesh<MeshImpl> glTriMesh; // wraps the mesh and draws it
		if (glTriMesh.m == NULL){
			glTriMesh.m = mpMesh;
		}
		vcg::tri::UpdateNormals<MeshImpl>::PerFace(*mpMesh);
		vcg::tri::UpdateNormals<MeshImpl>::PerVertexFromCurrentFaceNormal(*mpMesh);

		glTriMesh.Update();
		glTriMesh.Draw<vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone> ();

		/*
		glPushAttrib(GL_LIGHTING_BIT);
		glPushAttrib(GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glColor3f(0,0,0);
		glTriMesh.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
		glPopAttrib();
		glPopAttrib();
		*/
	}

	MeshImpl* Mesh::impl(){
		return mpMesh;
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Icosahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Icosahedron<MeshImpl>(*(m->mpMesh));

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*(m->mpMesh));
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*(m->mpMesh));

		return boost::shared_ptr<Mesh>(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Sphere(){
		Mesh* m = new Mesh();
		vcg::tri::Sphere<MeshImpl>(*(m->mpMesh));

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*(m->mpMesh));
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*(m->mpMesh));


		return boost::shared_ptr<Mesh>(m);
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Mesh& mesh){
	return o << "mesh " << &mesh;
}

std::ostream& operator<<(std::ostream& o, const fg::VertexProxy& v){
	return o << "vertex " << &v;
}
