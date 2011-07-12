#ifndef FG_MESHNODE_H
#define FG_MESHNODE_H

#include <boost/shared_ptr.hpp>

#include "fg/node.h"
#include "fg/mesh.h"

namespace fg {
	class MeshNode: public Node {
	public:
		MeshNode(boost::shared_ptr<Mesh> m):mMesh(m){}
		boost::shared_ptr<Mesh> mesh(){return mMesh;}
	protected:
		boost::shared_ptr<Mesh> mMesh;
	};
}
#endif
