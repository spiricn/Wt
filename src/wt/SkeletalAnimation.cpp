#include "wt/stdafx.h"

#include "wt/SkeletalAnimation.h"

namespace wt{

SkeletalAnimation::SkeletalAnimation(Model* model, SkeletonBone* skeleton, Animation* animation) : mModel(model), mAnimation(animation){
	setSource(model, skeleton, animation);
}

SkeletalAnimation::SkeletalAnimation() : mModel(NULL), mAnimation(NULL), mBoneAnimations(NULL), mSkeleton(NULL){
}

SkeletalAnimation::~SkeletalAnimation(){
}

SkeletalAnimation::BoneAnimation* SkeletalAnimation::getBoneAnimations(){
	return mBoneAnimations.getData();
}

void SkeletalAnimation::setSource(Model* model, SkeletonBone* skeleton, Animation* animation){
	mModel = model;
	mSkeleton = skeleton;
	mAnimation = animation;
}

Animation* SkeletalAnimation::getAnimation(){
	return mAnimation;
}

Uint32 SkeletalAnimation::getNumBoneAnimations() const{
	return mBoneAnimations.getCapacity();
}

float SkeletalAnimation::getDuration() const{
	return mAnimation->getDuration();
}

void SkeletalAnimation::create(){
	mBoneAnimations.create( mAnimation->mNodeAnimations.size() );

	// pair up animation nodes to our bone animations
	for(Uint32 i=0; i<mAnimation->mNodeAnimations.size(); i++){
		NodeAnimation* animationNode = mAnimation->mNodeAnimations[i];
		SkeletonBone* animatedBone = mSkeleton->findChildByName(animationNode->getTargetNode(), true);

		WT_ASSERT(animatedBone != NULL, "Unable to match node to bone");

		mBoneAnimations[i] = BoneAnimation(animatedBone, animationNode);
	}
}

} // </wt>
