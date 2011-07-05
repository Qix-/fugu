#include "fg/node.h"

namespace fg {

	Node::Node():mTransform(Mat4::Identity()){}
	Node::~Node(){}

	Mat4& Node::getTransform(){return mTransform;}
}
