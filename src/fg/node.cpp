#include "fg/node.h"

namespace fg {

	Node::Node()
	:mRelativeTransform(Mat4::Identity())
	,mCompoundTransform(Mat4::Identity())
	,mHasCompoundTransformBeenApplied(false)
	,mGraphIndex(-1)
	{}

	Node::~Node(){}

	Mat4& Node::getRelativeTransform(){
		return mRelativeTransform;
	}

	Mat4& Node::getCompoundTransform(){
		return mCompoundTransform;
	}

	void Node::resetCompoundTransform(){
		mHasCompoundTransformBeenApplied = false;
	}

	void Node::applyCompoundTransform(){
		mCompoundTransform = Mat4(mRelativeTransform);
		mHasCompoundTransformBeenApplied = true;
	}

	void Node::applyCompoundTransform(const Mat4& mParentTransform){
		mCompoundTransform = mParentTransform * mRelativeTransform;
		mHasCompoundTransformBeenApplied = true;
	}

	bool Node::hasCompoundTransformBeenApplied() const {
		return mHasCompoundTransformBeenApplied;
	}

	void Node::_setGraphIndex(int gi){
		mGraphIndex = gi;
	}

	int Node::_getGraphIndex() const {
		return mGraphIndex;
	}
}

std::ostream& operator<<(std::ostream& o, const fg::Node& n){
	return o << "Node@" << &n
			<< ":\nRT:\n" << n.mRelativeTransform
			<< ":\nCT:\n" << n.mCompoundTransform << "\n";
}
