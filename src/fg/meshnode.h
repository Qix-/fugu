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

#ifndef FG_MESHNODE_H
#define FG_MESHNODE_H

#include <boost/shared_ptr.hpp>

#include "fg/node.h"
#include "fg/mesh.h"

// forward decl
namespace fg {class MeshNode;}
std::ostream& operator<<(std::ostream& o, const fg::MeshNode& n);

namespace fg {
	/**
	 * \brief A fg::Node which references a fg::Mesh
	 */
	class MeshNode: public Node {
	public:
		MeshNode(boost::shared_ptr<Mesh> m):mMesh(m){}
		boost::shared_ptr<Mesh> mesh(){return mMesh;}

		void setMesh(boost::shared_ptr<Mesh> m){mMesh = m;}

		friend std::ostream& (::operator <<)(std::ostream& o, const fg::MeshNode& n);
	protected:
		boost::shared_ptr<Mesh> mMesh;
	};
}
#endif
