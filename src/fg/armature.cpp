#include "fg/armature.h"

namespace fg {

	Bone::Bone()
	:mParent()
	,mChildren()

	,mInitialPosition(0,0,0)
	,mInitialOrientation(Vec3(1,0,0),0)
	,mLength(1)

	,mCurrentOrientation(Vec3(0,0,1),0)

	{}

	Bone::Bone(BoneWeakRef parent)
	:mParent(parent)
	,mChildren()

	,mInitialPosition(0,0,0)
	,mInitialOrientation(Vec3(1,0,0),0)
	,mLength(1)

	,mCurrentOrientation(Vec3(0,0,1),0)

	{}

	/*
	void Bone::update(Mat4 parentSpace){
		mRelTransform.setTranslate(mRelPosition);
		mRelTransform = mRelTransform * mRelOrientation.toMat4();
		mWorldSpaceTransform = parentSpace*mRelTransform;

		foreach(BoneWeakRef b, mChildren){
			BoneRef br = b.lock();
			Mat4 lengthTranslation;
			lengthTranslation.setTranslate(Vec3(mLength,0,0));
			br->update(mWorldSpaceTransform*lengthTranslation);
		}
	}
	*/

	void Bone::computeCurrentWorldSpaceTransforms(Mat4 parentWS){
		// Compute transforms Object and Object->World transforms
		mCOTransform.setTranslate(mInitialPosition);
		mCOTransform= mCOTransform * mCurrentOrientation.toMat4();
		mCWSTransform = parentWS*mCOTransform;

		foreach(BoneWeakRef b, mChildren){
			BoneRef br = b.lock();
			Mat4 lengthTranslation;
			lengthTranslation.setTranslate(Vec3(mLength,0,0));
			br->computeCurrentWorldSpaceTransforms(mCWSTransform*lengthTranslation);
		}
	}


	void Bone::computeInitialWorldSpaceTransforms(Mat4 parentIWS, Mat4 invParentIWS){
		// Compute transforms Object and Object->World transforms
		mIOTransform.setTranslate(mInitialPosition);
		mIOTransform = mIOTransform * mInitialOrientation.toMat4();
		mIWSTransform = parentIWS*mIOTransform;

		// Compute inverses
		Mat4 inverseTrans;
		inverseTrans.setTranslate(-mInitialPosition);
		mInverseIO = mInitialOrientation.inverse().toMat4();
		mInverseIO = mInverseIO*inverseTrans;
		mInverseIWS = mInverseIO*invParentIWS;

		//
		foreach(BoneWeakRef b, mChildren){
			BoneRef br = b.lock();
			Mat4 lengthTranslation;
			lengthTranslation.setTranslate(Vec3(mLength,0,0));

			Mat4 invLengthTranslate;
			invLengthTranslate.setTranslate(Vec3(-mLength,0,0));

			br->computeInitialWorldSpaceTransforms(mIWSTransform*lengthTranslation,invLengthTranslate*mInverseIWS);
		}
	}

	Armature::Armature()
	:mBones()
	{
		mRootBone = BoneRef(new Bone());
		mRootBone->setLength(0);

		mBones.push_back(mRootBone);
	}

	BoneRef Armature::root(){
		return mRootBone;
	}

	const std::list<BoneRef>& Armature::bones() const {
		return mBones;
	}

	void Armature::addBone(BoneRef b){
		mBones.push_back(b);
	}

	void Armature::update(){
		mRootBone->computeCurrentWorldSpaceTransforms();

		// for now...
		// mRootBone->computeInitialWorldSpaceTransforms();
	}

}
