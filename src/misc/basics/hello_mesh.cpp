#include "fg/mesh.h"
#include "fg/util.h"

using namespace fg;

int main(){
	shared_ptr<Mesh> m = Mesh::Primitives::Icosahedron();
	shared_ptr<Mesh::VertexSet> all = m->selectAllVertices();
	foreach(shared_ptr<VertexProxy> v, *all){
		std::cout << v->getNumBones() << " ";
	}
	return 0;
}
