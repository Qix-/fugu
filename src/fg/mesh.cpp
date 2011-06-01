#include "fg/mesh.h"

#include <vcg/complex/algorithms/smooth.h>

#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/extended_marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>



using namespace vcg;

namespace fg {
	Mesh::Mesh()
	:mMesh()
	{

	}

	Mesh::~Mesh(){
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