#include "fg/armature.h"

namespace fg {

	Bone::Bone()
	:mParent()
	,mChildren()

	,mRelPosition(0,0,0)
	,mRelOrientation(Vec3(1,0,0),0)
	,mLength(1)

	,mRelTransform()
	,mWorldSpaceTransform()
	{}

	Bone::Bone(BoneWeakRef parent)
	:mParent(parent)
	,mChildren()

	,mRelPosition(0,0,0)
	,mRelOrientation(Vec3(1,0,0),0)
	,mLength(1)

	,mRelTransform()
	,mWorldSpaceTransform()
	{}

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
		mRootBone->update();
	}

}
