#include "wt/stdafx.h"

#include "wt/SkeletalAnimationPlayer.h"

#define TD_TRACE_TAG "SkeletonAnimationPlayer"


namespace wt{

SkeletalAnimationPlayer::SkeletalAnimationPlayer(Model* model) : mModel(model), mAnimationTime(0.0f),
	mAnimationSpeed(1.0f), mIsLooping(false), mCurrentAnimation(NULL), mTimeAccumulator(1/30.0f) {

	Uint32 numBones = model->getRootBone()->getNumChildren(true) /* number of roots children */ + 1 /* root itself */;

	/// Initialize everything to identitiy matrices
	mLocalBoneTransforms.create(numBones);

	mGlobalBoneTransforms.create(numBones);

	mGlobalTransforms.create(numBones);

	initTransforms(model->getRootBone());
}

void SkeletalAnimationPlayer::initTransforms(SkeletonBone* bone){
		mLocalBoneTransforms[bone->getIndex()] = bone->getTransform();

		bone->getGlobalTransform( mGlobalTransforms[bone->getIndex()] );

		mGlobalBoneTransforms[bone->getIndex()] = mGlobalTransforms[bone->getIndex()] * bone->getOffset();

		for(Uint32 i=0; i<bone->getNumChildren(false); i++){
			initTransforms(bone->getChildAt(i));
		}
	}

SkeletalAnimationPlayer::~SkeletalAnimationPlayer(){
}

void SkeletalAnimationPlayer::evaluate(){
	Uint32 numChannels = mCurrentAnimation->getNumBoneAnimations();

	// For each node(bone) animation)
	for(Uint32 i=0; i<numChannels; i++){
		// Bone we're animating
		SkeletonBone* bone = mCurrentAnimation->getBoneAnimations()[i].bone;

		// Bone's paired up node animation
		NodeAnimation* boneAnimation = mCurrentAnimation->getBoneAnimations()[i].animation;		

		// Evaluate its current state and save it
		boneAnimation->evaluate( mAnimationTime, mLocalBoneTransforms[bone->getIndex()] );
	}

	// After all the local bone transformations have been recalculated, calculate the global ones
	// starting with the root bone (skeleton)
	calculateGlobalTransforms( mModel->getSkeleton() );
}

void SkeletalAnimationPlayer::calculateGlobalTransforms(SkeletonBone* bone){
	// Reference to this bones global transform matrix
	glm::mat4& globalTransform = mGlobalTransforms[ bone->getIndex() ] ;
		
	// Bone global transform is a concatenation of its local transform and its parents global transform
	// so first get its local transform ...
	globalTransform = mLocalBoneTransforms[ bone->getIndex() ];

	if(bone->getParent() != NULL){
		// and the if it has a parent, concatenate it with the parents transformation
		globalTransform = mGlobalTransforms[ bone->getParent()->getIndex() ] * globalTransform;
	}

	// After offsetting it (with the offset matrix) write it to global transform cache
	mGlobalBoneTransforms[bone->getIndex()] = globalTransform * bone->getOffset();

	// Do the same for all its children recursivley
	for(Uint32 i=0; i<bone->getNumChildren(false); i++){
		calculateGlobalTransforms(bone->getChildAt(i));
	}
}

void SkeletalAnimationPlayer::play(const String& name, bool loop){
	play(mModel->getSkeletalAnimation(name), loop);
}

void SkeletalAnimationPlayer::play(SkeletalAnimation* animation, bool loop){
	mCurrentAnimation = animation;
	mAnimationTime = 0.0f;
	mIsLooping = loop;
}

void SkeletalAnimationPlayer::advance(float delta){
	if(!isPlaying()){
		return;
	}

	if(!mTimeAccumulator.update(delta)){
		return;
	}

	float dt = mTimeAccumulator.getStep();
	
	bool animationFinished=false;
	if(mIsLooping){
		mAnimationTime = fmod(mAnimationTime  + dt*mAnimationSpeed, mCurrentAnimation->getDuration());
	}
	else{
		mAnimationTime = mAnimationTime  + dt*mAnimationSpeed;
		if(mAnimationTime >= mCurrentAnimation->getDuration()){
			mAnimationTime = mCurrentAnimation->getDuration();
			animationFinished = true;
		}
	}

	evaluate();

	if(animationFinished){
		mCurrentAnimation = NULL;
	}
}

void SkeletalAnimationPlayer::setPosition(float pos){
	mAnimationTime = mCurrentAnimation->getDuration() * pos;
}

bool SkeletalAnimationPlayer::isPlaying() const{
	return mCurrentAnimation != NULL;
}

void SkeletalAnimationPlayer::setSpeed(float speed){
	mAnimationSpeed = speed;
}

const glm::mat4* SkeletalAnimationPlayer::getBoneMatrices() const{
	return mGlobalBoneTransforms.getData();
}

Uint32 SkeletalAnimationPlayer::getNumBones() const{
	return mGlobalBoneTransforms.getCapacity();
}

}; // </wt>
