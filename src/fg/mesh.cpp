/**
 * \file
 * \brief Defines fg::Mesh
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

#include "fg/mesh.h"
#include "fg/meshimpl.h"
#include "fg/functions.h"
#include "fg/util.h"

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
#include <vcg/complex/algorithms/update/position.h>

#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/simplex/face/component_ocf.h>

#include <wrap/io_trimesh/import.h>


#include <boost/foreach.hpp>

using namespace vcg;

namespace fg {


	Mesh::Mesh()
	:mpMesh(NULL)
	,mVertexProxyList()
	,mFaceProxyList()
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
				r->push_back(_newSP(&v));
				//boost::shared_ptr<VertexProxy> pvp(new VertexProxy(this, &v));
				//r->push_back(pvp);
				//mVertexProxyList.add(pvp);
			}
		}
		return r;
	}

	boost::shared_ptr<Mesh::FaceSet> Mesh::selectAllFaces(){
		boost::shared_ptr<FaceSet> r(new FaceSet());
		BOOST_FOREACH(FaceImpl& f, mpMesh->face){
			// only return non-dead vertices
			if (!f.IsD()){
				r->push_back(_newSP(&f));
			}
		}
		return r;
	}

	boost::shared_ptr<VertexProxy> Mesh::selectRandomVertex(){
		return _newSP(&mpMesh->vert[(int)(fg::random(0,mpMesh->vert.size()))]);
	}


	boost::shared_ptr<FaceProxy> Mesh::selectRandomFace(){
		return _newSP(&mpMesh->face[(int)(fg::random(0,mpMesh->face.size()))]);
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

		// vcg::face::IsManifold<MeshImpl>;
		// vcg::face::IsBorder<MeshImpl>;
		// vcg::face::IsManifold<MeshImpl::FaceType>(mMesh.face[0], 0);

		for(int i=0;i<levels;i++)
			vcg::Refine(*mpMesh,vcg::MidPoint<MeshImpl>(mpMesh));

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*mpMesh);
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*mpMesh);

		sync();
	}

	void Mesh::smoothSubdivide(int levels){
		if (levels <= 0) return;

		// TODO

		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*mpMesh);
		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*mpMesh);

		// vcg::face::IsManifold<MeshImpl>;
		// vcg::face::IsBorder<MeshImpl>;
		// vcg::face::IsManifold<MeshImpl::FaceType>(mMesh.face[0], 0);

		for(int i=0;i<levels;i++){
			vcg::Refine(*mpMesh,vcg::MidPointButterfly<MeshImpl>());
			//vcg::Refine(*mpMesh,vcg::MidPoint<MeshImpl>(mpMesh));
		}

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*mpMesh);
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*mpMesh);

		sync();
	}

	void Mesh::drawGL(){
		static vcg::GlTrimesh<MeshImpl> glTriMesh; // wraps the mesh and draws it
		if (glTriMesh.m == NULL){
			glTriMesh.m = mpMesh;
		}
		sync();

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


	void Mesh::sync(){
		vcg::tri::UpdateNormals<MeshImpl>::PerFace(*mpMesh);
		vcg::tri::UpdateNormals<MeshImpl>::PerVertexFromCurrentFaceNormal(*mpMesh);
	}

	void Mesh::applyTransform(const Mat4& T){
		vcg::tri::UpdatePosition<MeshImpl>::Matrix(*mpMesh,T,true);
		/*
		foreach(VertexImpl& v, mpMesh->vert){
			// only return non-dead vertices
			if (!v.IsD()){
				v.P() = T*v.P();
			}
		}
		sync();
		*/
	}

	MeshImpl* Mesh::_impl(){
		return mpMesh;
	}

	shared_ptr<VertexProxy> Mesh::_newSP(VertexImpl* v){
		shared_ptr<VertexProxy> pvp(new VertexProxy(this, v));
		mVertexProxyList.add(pvp);
		return pvp;
	}

	shared_ptr<FaceProxy> Mesh::_newSP(FaceImpl* f){
		shared_ptr<FaceProxy> pvp(new FaceProxy(this, f));
		mFaceProxyList.add(pvp);
		return pvp;
	}

	VertexProxyList* Mesh::_vpl(){
		return &mVertexProxyList;
	}

	FaceProxyList* Mesh::_fpl(){
		return &mFaceProxyList;
	}


	// PRIMITIVES

	boost::shared_ptr<Mesh> Mesh::Primitives::Cube(){
		Mesh* m = new Mesh();
		// A Hexahedron is a Cube!
		vcg::tri::Hexahedron<MeshImpl>(*(m->mpMesh));
		// vcg::tri::Box<MeshImpl>(*(m->mpMesh),vcg::Box3<double>(vcg::Point3<double>(-1,-1,-1),vcg::Point3<double>(1,1,1)));
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Icosahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Icosahedron<MeshImpl>(*(m->mpMesh));
		Mat4 s; s.setScale(1/1.7,1/1.7,1/1.7);
		m->applyTransform(s);
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Sphere(){
		Mesh* m = new Mesh();
		vcg::tri::Sphere<MeshImpl>(*(m->mpMesh));
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Tetrahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Tetrahedron<MeshImpl>(*(m->mpMesh));
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Dodecahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Dodecahedron<MeshImpl>(*(m->mpMesh));
		Mat4 s; s.setScale(1/4.2,1/4.2,1/4.2);
		m->applyTransform(s);
		return sync(m);
	}
	boost::shared_ptr<Mesh> Mesh::Primitives::Octahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Octahedron<MeshImpl>(*(m->mpMesh));
		return sync(m);
	}
	/*
	boost::shared_ptr<Mesh> Mesh::Primitives::Hexahedron(){
		Mesh* m = new Mesh();
		vcg::tri::Hexahedron<MeshImpl>(*(m->mpMesh));
		return sync(m);
	}
	*/
	boost::shared_ptr<Mesh> Mesh::Primitives::Cone(double r1, double r2, const int SubDiv){
		Mesh* m = new Mesh();
		vcg::tri::Cone<MeshImpl>(*(m->mpMesh), r1, r2, 2, SubDiv);
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::Cylinder(int slices){
		Mesh* m = new Mesh();
		vcg::tri::Cone<MeshImpl>(*(m->mpMesh), 1, 1, 2, slices);
		//vcg::tri::Cylinder<MeshImpl>(slices,stacks,*(m->mpMesh));
		return sync(m);
	}

	boost::shared_ptr<Mesh> Mesh::Primitives::sync(Mesh* m){
		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*(m->mpMesh));
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*(m->mpMesh));
		m->sync();
		return boost::shared_ptr<Mesh>(m);
	}

	// MESHBUILDER

	// std::vector<Vec3> mVertices;
	// std::vector<boost::tuple<int,int,int> > mTriangles;

	Mesh::MeshBuilder::MeshBuilder()
	:mVertices()
	,mVertexColours()
	,mVertexUVs()
	,mTriangles(){}

	Mesh::MeshBuilder::~MeshBuilder(){

	}

	int Mesh::MeshBuilder::getNumVertices(){
		return mVertices.size();
	}

	int Mesh::MeshBuilder::getNumVerticies(){
		return mVertices.size();
	}

	void Mesh::MeshBuilder::addVertex(double x, double y, double z){
		mVertices.push_back(Vec3(x,y,z));
	}

	void Mesh::MeshBuilder::addVertex(Vec3 pos){
		mVertices.push_back(pos);
	}

	void Mesh::MeshBuilder::addVertexColour(Vec3 colour){
		mVertexColours.push_back(colour);
	}

	void Mesh::MeshBuilder::addVertexUV(double u, double v){
		mVertexUVs.push_back(make_pair(u,v));
	}


	void Mesh::MeshBuilder::addFace(int v1, int v2, int v3){
		mTriangles.push_back(boost::make_tuple(v1,v2,v3));
	}

	boost::shared_ptr<Mesh> Mesh::MeshBuilder::createMesh(){
		Mesh* m = new Mesh();

		int numcols = mVertexColours.size();
		int numuvs = mVertexUVs.size();

		vcg::tri::Allocator<MeshImpl>::AddVertices(*m->_impl(),mVertices.size());
		for(int i=0;i<mVertices.size();i++){
			m->_impl()->vert[i].P() = mVertices[i];

			if (i < numcols){
				vcg::Color4b &c = m->_impl()->vert[i].C();
				Vec3& col = mVertexColours[i];
				c.X() = 255*col.X();
				c.Y() = 255*col.Y();
				c.Z() = 255*col.Z();
			}

			if (i < numuvs){
				// Set UV's...
				m->_impl()->vert[i].T().U() = mVertexUVs[i].first;
				m->_impl()->vert[i].T().V() = mVertexUVs[i].second;
			}
		}

		vcg::tri::Allocator<MeshImpl>::AddFaces(*m->_impl(),mTriangles.size());
		for(int i=0;i<mTriangles.size();i++){
			FaceImpl* f = &m->_impl()->face[i];
			f->V(0) = &m->_impl()->vert[mTriangles[i].get<0>()];
			f->V(1) = &m->_impl()->vert[mTriangles[i].get<1>()];
			f->V(2) = &m->_impl()->vert[mTriangles[i].get<2>()];
		}

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*(m->_impl()));
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*(m->_impl()));
		m->sync();
		return boost::shared_ptr<Mesh>(m);
	}

	boost::shared_ptr<Mesh> Mesh::MeshBuilder::createMesh(const std::vector<Vec3>& verts, const std::vector<boost::tuple<int,int,int> >& faces)
	{
		Mesh* m = new Mesh();

		vcg::tri::Allocator<MeshImpl>::AddVertices(*m->_impl(),verts.size());
		for(int i=0;i<verts.size();i++){
			m->_impl()->vert[i].P() = verts[i];
		}

		vcg::tri::Allocator<MeshImpl>::AddFaces(*m->_impl(),faces.size());
		for(int i=0;i<faces.size();i++){
			FaceImpl* f = &m->_impl()->face[i];
			f->V(0) = &m->_impl()->vert[faces[i].get<0>()];
			f->V(1) = &m->_impl()->vert[faces[i].get<1>()];
			f->V(2) = &m->_impl()->vert[faces[i].get<2>()];
		}

		vcg::tri::UpdateTopology<MeshImpl>::VertexFace(*(m->_impl()));
		vcg::tri::UpdateTopology<MeshImpl>::FaceFace(*(m->_impl()));
		m->sync();
		return boost::shared_ptr<Mesh>(m);
	}

	boost::shared_ptr<Mesh> Mesh::Load(std::string path){
		return Load(boost::filesystem::path(path));
	}

	boost::shared_ptr<Mesh> Mesh::Load(boost::filesystem::path file){
		_FloatMeshImpl fm;
		std::string filepath = file.string();
		vcg::tri::io::Importer<_FloatMeshImpl>::Open(fm,filepath.c_str());

		// copy
		Mesh* m = new Mesh();
		_copyFloatMeshIntoMesh(fm,*m->_impl());
		m->sync();
		return boost::shared_ptr<Mesh>(m);
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Mesh& mesh){
	return o << "mesh " << &mesh;
}
