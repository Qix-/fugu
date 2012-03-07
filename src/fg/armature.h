/**
 * \file
 * \brief Armatures are used in skeletal animation. NOTE: These are not incorporated into fg yet.
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
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

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
	 * A bone has an initial pose, and a current pose. The initial pose
	 * is typically set once and is used to calculate the rest state of
	 * bound vertices. The current pose represents the current state of
	 * the bone.
	 *
	 * By default a bone points in the (1,0,0) direction
	 *
	 * TODO: Add user-specified data
	 */
	class Bone {
		public:

		Bone();
		Bone(BoneWeakRef parent);

		/// \brief recursively calculate the current world-space transform for self and descendants
		void computeCurrentWorldSpaceTransforms(Mat4 parentSpace = Mat4::Identity());
		Mat4 getCWSTransform() const {return mCWSTransform;}

		/// \brief recursively calculate the initial world-space transform for self and descendants
		void computeInitialWorldSpaceTransforms(Mat4 parentIWS = Mat4::Identity(), Mat4 invParentIWS = Mat4::Identity());
		Mat4 getIWSTransform() const {return mIWSTransform;}
		Mat4 getInvIWSTransform() const {return mInverseIWS;}

		void addChild(BoneWeakRef child){mChildren.push_back(child);}

		// These functions affect the base transform of the bone
		// which is used to compute the rest positions of the bound verts
		double getLength(){return mLength;}
		void setLength(double l){mLength = l;}

		Vec3 getInitialPosition(){return mInitialPosition;}
		void setInitialPosition(Vec3 v){mInitialPosition = v;}

		Quat getInitialOrientation(){return mInitialOrientation;}

		/// \brief set the starting orientation for this bone (also sets the current orientation to be the same)
		void setInitialOrientation(Quat q){mInitialOrientation = q; mCurrentOrientation = q;}

		Quat getCurrentOrientation(){return mCurrentOrientation;}
		void setCurrentOrientation(Quat q){mCurrentOrientation = q;}

		// Mat4 getPoseSpaceTransform(){return mRelTransform;}
		// Mat4 getWorldSpaceTransform(){return mWorldSpaceTransform;}

		// protected:
		BoneWeakRef mParent;
		std::list<BoneWeakRef> mChildren;

		// relative to parent
		Vec3 mInitialPosition;
		Quat mInitialOrientation;
		double mLength; // Length of this bone

		// pose transforms, applied to the initial coordinate system
		Quat mCurrentOrientation;

		// derived
		// Mat4 mRelTransform;

		Mat4 mIOTransform; // initial object transform
		Mat4 mIWSTransform; // object->world space transform for initial pose

		Mat4 mInverseIO; // inverse
		Mat4 mInverseIWS; // inverse

		Mat4 mCOTransform; // current object transform
		Mat4 mCWSTransform; // current world-space transform

		// computed worldspace transform
		// Mat4 mWorldSpaceTransform;
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
