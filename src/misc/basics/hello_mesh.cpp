#include "fg/mesh.h"

#include <boost/shared_ptr.hpp>

int main(){
	boost::shared_ptr<fg::Mesh> m = fg::Mesh::Primitives::Icosahedron();
	return 0;
}
