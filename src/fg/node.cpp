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

#include "fg/node.h"

namespace fg {

	Node::Node()
	:mRelativeTransform(Mat4::Identity())
	,mCompoundTransform(Mat4::Identity())
	,mHasCompoundTransformBeenApplied(false)
	,mGraphIndex(-1)
	,mDirty(true)
	{}

	Node::~Node(){}

	const Mat4& Node::getRelativeTransform() const {
		return mRelativeTransform;
	}

	void Node::setRelativeTransform(const Mat4& m){
		mRelativeTransform = m;
		mDirty = true;
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
		//mDirty = false; (dirtyness should propagate, so don't set this yet)
	}

	void Node::applyCompoundTransform(const Mat4& mParentTransform){
		mCompoundTransform = mParentTransform * mRelativeTransform;
		mHasCompoundTransformBeenApplied = true;
		//mDirty = false;
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
			<< "\nRT:\n" << n.mRelativeTransform
			<< "\nCT:\n" << n.mCompoundTransform
			<< "dirty=" << (n.mDirty?"true":"false") << "\n"
			<< "hasCompoundTransformBeenApplied=" << (n.mHasCompoundTransformBeenApplied?"true":"false") << "\n"
			<< "\n";
}
