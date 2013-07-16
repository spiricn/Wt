#include "wt/stdafx.h"

#include "wt/SkeletalAnimationPlayer.h"

#define TD_TRACE_TAG "SkeletonAnimationPlayer"


namespace wt{

SkeletalAnimationPlayer::SkeletalAnimationPlayer(Model* model) : mModel(model), mAnimationTime(0.0f),
	mAnimationSpeed(1.0f), mIsLooping(false), mCurrentAnimation(NULL), mTimeAccumulator(1/30.0f) {

	uint32_t numBones = model->getRootBone()->getNumChildren(true) /* number of roots children */ + 1 /* root itself */;

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

	for(uint32_t i=0; i<bone->getNumChildren(false); i++){
		initTransforms(bone->getChildAt(i));
	}
}

float SkeletalAnimationPlayer::getSpeed() const{
	return mAnimationSpeed;
}

float SkeletalAnimationPlayer::getPosition() const{
	return mAnimationTime;
}

SkeletalAnimationPlayer::~SkeletalAnimationPlayer(){
}

void SkeletalAnimationPlayer::evaluate(){
	uint32_t numChannels = mCurrentAnimation->getNumBoneAnimations();

	// For each node(bone) animation)
	for(uint32_t i=0; i<numChannels; i++){
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
	for(uint32_t i=0; i<bone->getNumChildren(false); i++){
		calculateGlobalTransforms(bone->getChildAt(i));
	}
}

const glm::mat4& SkeletalAnimationPlayer::getGlobalBoneTransform(uint32_t index) const{
	return mGlobalTransforms[index];
}

SkeletalAnimation* SkeletalAnimationPlayer::getCurrentAnimation() const{
	return mCurrentAnimation;
}

bool SkeletalAnimationPlayer::isLooping() const{
	return mIsLooping;
}

const String& SkeletalAnimationPlayer::getCurrentAnimationName() const{
	return mCurrentAnimationName;
}

void SkeletalAnimationPlayer::play(const String& name, bool loop){
	mCurrentAnimationName = name;
	play(mModel->getSkeletalAnimation(name), loop);
}

void SkeletalAnimationPlayer::play(SkeletalAnimation* animation, bool loop){
	if(!animation){
		mCurrentAnimationName = "";
	}

	// Reset matrices
	initTransforms(mModel->getRootBone());

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

uint32_t SkeletalAnimationPlayer::getNumBones() const{
	return mGlobalBoneTransforms.getCapacity();
}

}; // </wt>
