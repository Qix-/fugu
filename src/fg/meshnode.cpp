#include "fg/meshnode.h"

std::ostream& operator<<(std::ostream& o, const fg::MeshNode& n){
	return o << "MeshNode@" << &n
			<< "\nmesh:" << n.mMesh << "\n"
			<< "\nRT:\n" << n.mRelativeTransform
			<< "\nCT:\n" << n.mCompoundTransform
			<< "dirty=" << (n.mDirty?"true":"false") << "\n"
			<< "hasCompoundTransformBeenApplied=" << (n.mHasCompoundTransformBeenApplied?"true":"false") << "\n"
			<< "\n";
}
