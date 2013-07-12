#ifndef WT_SKELETALANIMATION_H
#define WT_SKELETALANIMATION_H

#include "wt/SkeletonBone.h"
#include "wt/Animation.h"
#include "wt/Buffer.h"

namespace wt{

class Model;

class SkeletalAnimation{
public:
	struct BoneAnimation{
		SkeletonBone* bone;
		NodeAnimation* animation;

		BoneAnimation(SkeletonBone* bone=NULL, NodeAnimation* animation=NULL) : bone(bone), animation(animation){
		}
	}; // </BoneAnimation>

private:
	Model* mModel;
	Animation* mAnimation;
	SkeletonBone* mSkeleton;
	// NodeAnimation to SkeletonBone map (each node is animating a single bone)
	Buffer<BoneAnimation> mBoneAnimations;

public:
	SkeletalAnimation();

	SkeletalAnimation(Model* model, SkeletonBone* skeleton, Animation* animation);

	~SkeletalAnimation();

	BoneAnimation* getBoneAnimations();

	void setSource(Model* model, SkeletonBone* skeleton, Animation* animation);

	Animation* getAnimation();

	Uint32 getNumBoneAnimations() const;

	float getDuration() const;

	void create();

}; //</SkeletalAnimation>


} // </wt>

#endif // </WT_SKELETALANIMATION_H>