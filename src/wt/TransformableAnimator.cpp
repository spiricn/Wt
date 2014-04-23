#include "wt/stdafx.h"

#include "wt/TransformableAnimator.h"

#define TD_TRACE_TAG "TransformableAnimator"

namespace wt
{

TransformableAnimator::TransformableAnimator(ATransformable* target, NodeAnimation* animation, bool loop, bool selfDestruct) : mTarget(target),
	mAnimation(animation), mCurrentTime(0.0f), mLoop(loop), mState(eSTATE_PLAYING), mListener(NULL), mSpeed(1.0f), mSelfDestruct(selfDestruct), mAttribs(0xFFFFFFFF){
}

TransformableAnimator::TransformableAnimator(ATransformable* target, Animation* animation, const String& node, bool loop, bool selfDestruct) : mTarget(target),
	mAnimation(NULL), mCurrentTime(0.0f), mLoop(loop), mState(eSTATE_PLAYING), mListener(NULL), mSpeed(1.0f), mSelfDestruct(selfDestruct), mAttribs(0xFFFFFFFF){
		mAnimation = animation->findNodeAnimation(node);

		WT_ASSERT(mAnimation != NULL, "Invalid node animation name \"%s\" for animatino \"%s\"", node.c_str(), animation->getPath().c_str());
}

TransformableAnimator::State TransformableAnimator::getState() const{
	return mState;
}

NodeAnimation* TransformableAnimator::getNodeAnimation() const{
	return mAnimation;
}

void TransformableAnimator::pause(){
	if(mState == eSTATE_PLAYING){
		changeState( eSTATE_PAUSED );
	}
	else{
		TRACEE("Invlaid state");
	}
}

void TransformableAnimator::setSpeed(float speed){
	mSpeed = speed;
}

void TransformableAnimator::reset(){
	changeState(eSTATE_PLAYING);
	mCurrentTime = 0.0f;
}

float TransformableAnimator::getPosition() const{
	return mCurrentTime;
}

void TransformableAnimator::setPosition(float pos){
	if(pos < 0.0f || pos > mAnimation->getDuration()){
		TRACEE("Invalid position time %f", pos);
		return;
	}

	if(mCurrentTime == pos){
		return;
	}
	
	mCurrentTime = pos;
	animate();

	/*if(mListener){
		mListener->onAnimationProgress(this, mCurrentTime);
	}*/
}

void TransformableAnimator::play(){
	if(mState == eSTATE_PAUSED){
		changeState( eSTATE_PLAYING );
	}
	else if(mState == eSTATE_FINISHED){
		mCurrentTime = 0.0f;
		changeState( eSTATE_PLAYING );
	}
	else{
		TRACEE("Invlaid state");
	}
}

void TransformableAnimator::setListener(IListener* listener){
	mListener = listener;
}

void TransformableAnimator::stop(){
	if(mState == eSTATE_PLAYING || mState == eSTATE_PAUSED){
		changeState(eSTATE_FINISHED);

		mCurrentTime = 0.0f;
		if(mListener){
			mListener->onAnimationProgress(this, mCurrentTime);
		}
	}
}

void TransformableAnimator::changeState(State state){
	State oldState = mState;
	mState = state;

	if(mListener && state != oldState){
		mListener->onAnimationStateChanged(this, state);
	}
}

void TransformableAnimator::onProcStart(){
}

void TransformableAnimator::onProcEnd(){
	changeState(eSTATE_FINISHED);
}

void TransformableAnimator::animate(){
	if(!mAttribs){
		return;
	}

	glm::vec3 animPos;
	glm::vec3 animScale;
	glm::quat animRotation;

	mAnimation->evaluate(mCurrentTime, animPos, animRotation, animScale);

	if( (mAttribs & eATTRIB_SCALE) == 0){
		// Don't animate scale
		mTarget->getScale(animScale);
	}

	if( (mAttribs & eATTRIB_ROTATION) == 0){
		// Don't animate rotation
		mTarget->getRotation(animRotation);
	}

	if( (mAttribs & eATTRIB_POSITION) == 0){
		// Don't animate position
		mTarget->getTranslation(animPos);
	}

	glm::mat4 mat(1.0f);

	mat = glm::scale(animScale);

	mat = glm::mat4_cast( animRotation ) * mat;

	mat = glm::translate(animPos) * mat;

	mTarget->setTransformMatrix(mat);
}

void TransformableAnimator::onProcUpdate(float dt){
	if(mState != eSTATE_PLAYING){
		return;
	}

	dt *= mSpeed;

	mCurrentTime += dt;

	bool finished = false;

	if(mLoop){
		mCurrentTime = glm::mod(mCurrentTime, mAnimation->getDuration());
	}
	else if(mCurrentTime >= mAnimation->getDuration()){
		mCurrentTime = mAnimation->getDuration();
		finished = true;
	}

	if(mListener){
		mListener->onAnimationProgress(this, mCurrentTime / mAnimation->getDuration() );
	}

	animate();

	if(finished){
		changeState( eSTATE_FINISHED );
		if(mSelfDestruct){
			killProcess();
		}
	}
}

void TransformableAnimator::setAnimationAttribs(AnimationAttrib attribs){
	mAttribs = attribs;
}

} // </wt>