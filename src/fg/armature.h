#ifndef FG_ARMATURE_H
#define FG_ARMATURE_H

#include "fg/fg.h"
#include "fg/functions.h"
// #include "fg/glrenderer.h"
#include "fg/quat.h"
#include "fg/mat4.h"
#include "fg/vec3.h"
#include "fg/util.h"

namespace fg {
	class Bone;
	typedef shared_ptr<Bone> BoneRef;
	typedef weak_ptr<Bone> BoneWeakRef;

	/**
	 * \brief A Bone is a primitive of an armature
	 *
	 * By default a bone points in the (1,0,0) direction
	 *
	 * TODO: Add user-specified data
	 *
	 */
	class Bone {
		public:

		Bone();
		Bone(BoneWeakRef parent);

		/** \brief recursively calculate world-space transform for self and descendants
		 */
		void update(Mat4 parentSpace = Mat4::Identity());

		void addChild(BoneWeakRef child){mChildren.push_back(child);}

		double getLength(){return mLength;}
		void setLength(double l){mLength = l;}

		Vec3 getPosition(){return mRelPosition;}
		void setPosition(Vec3 v){mRelPosition = v;}

		Quat getOrientation(){return mRelOrientation;}
		void setOrientation(Quat q){mRelOrientation = q;}

		Mat4 getPoseSpaceTransform(){return mRelTransform;}
		Mat4 getWorldSpaceTransform(){return mWorldSpaceTransform;}

		// protected:
		BoneWeakRef mParent;
		std::list<BoneWeakRef> mChildren;

		// relative to parent
		Vec3 mRelPosition;
		Quat mRelOrientation;
		double mLength; // Length of this bone

		// derived
		Mat4 mRelTransform;
		// computed worldspace transform
		Mat4 mWorldSpaceTransform;
	};

	/**
	 * \brief An armature is a collection of poseable bones arranged in a hierarchy. (It is also known as a "skeleton" in skeletal animation.)
	 *
	 * TODO: It can be *bound* to a fg::Mesh to animate vertex positions.
	 * TODO: Fit into Node system
	 */
	class Armature {
	public:
		Armature();
		~Armature(){}

		BoneRef root(); ///< Get the root bone
		const std::list<BoneRef>& bones() const; ///< Access all bones

		/**
		 * \brief Add a new bone to this armature
		 */
		void addBone(BoneRef);

		void update(); ///< Update the bone-space of everything

	protected:
		BoneRef mRootBone;
		std::list<BoneRef> mBones;
	};
}

#endif // FG_ARMATURE_H
