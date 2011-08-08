/**
 * \file
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

#include "fg/vertex.h"
#include "fg/meshimpl.h" // VertexImpl

namespace fg {
	VertexProxy::VertexProxy(VertexImpl* vi):Proxy<VertexImpl>(vi){}
	VertexProxy::~VertexProxy(){}

	Vec3 VertexProxy::getPos() const {
		return constImpl().P();
	}

	void VertexProxy::setPos(Vec3 v){
		pImpl()->P() = v;
	}

	void VertexProxy::setPos(double x, double y, double z){
		pImpl()->P().X() = x;
		pImpl()->P().Y() = y;
		pImpl()->P().Z() = z;
	}

	Vec3 VertexProxy::getN() {
		return pImpl()->N();
	}

	Vec3 VertexProxy::getColour() const {
		vcg::Color4b c = constImpl().C();
		Vec3 v(c.X()/255.,c.Y()/255.,c.Z()/255.);
		return v;
	}

	void VertexProxy::setColour(Vec3 k){
		vcg::Color4b c(k.getX()*255,k.getY()*255,k.getZ()*255,255);
		pImpl()->C() = c;
	}

	void VertexProxy::setColour(double r, double g, double b){
		pImpl()->C().X() = r*255.;
		pImpl()->C().Y() = g*255;
		pImpl()->C().Z() = b*255;
	}

	void VertexProxy::setUV(double u, double v){
		pImpl()->T().U() = u;
		pImpl()->T().V() = v;
	}

	int VertexProxy::getNumBones() const {
		return constImpl().getNumBones();
	}


	// VertexProxyList

	VertexProxyList::VertexProxyList():mProxies(){}
	VertexProxyList::~VertexProxyList(){}

	/**
	 * Add a new vertexproxy to this list. It will be removed when their are no
	 * more references to the vertexproxy (besides this one).		 *
	 */
	void VertexProxyList::add(boost::shared_ptr<VertexProxy> v){
		mProxies.push_back(v);
	}

	/**
	 * Remove any unreferenced proxies.
	 */
	void VertexProxyList::doGarbageCollection(){
		int count = 0;

		// Delete any proxy refs
		std::list<boost::shared_ptr<VertexProxy> >::iterator it = mProxies.begin();
		while(it!=mProxies.end()){
			std::cout << "usecount(" << *it << ") = " << it->use_count() << "\n";
			if (it->use_count()<=1){
				count++;
				it = mProxies.erase(it);
			}
			else
				it++;
		}

		if (count > 0)
			std::cout << "VertexProxyList: " << count << " proxies cleaned up\n";
	}

	/**
	 * Return a list of pointers to pointers to vcg::vertex,
	 * this will be used in the vcg Allocator to update the
	 * internal refs of VertexProxies
	 */
	std::vector<VertexImpl**> VertexProxyList::getUpdateList()
	{
		std::vector<VertexImpl**> vpl;
		BOOST_FOREACH(boost::shared_ptr<VertexProxy>& pvp, mProxies){
			vpl.push_back(&pvp->pImpl());
		}
		return vpl;
	}

} // namespace fg
