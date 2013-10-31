#include "wt/stdafx.h"

#include "wt/TransformableAnimator.h"

#define TD_TRACE_TAG "TransformableAnimator"

namespace wt
{

TransformableAnimator::TransformableAnimator(ATransformable* target, NodeAnimation* animation, bool loop) : mTarget(target),
	mAnimation(animation), mCurrentTime(0.0f), mLoop(loop), mState(eSTATE_PLAYING), mListener(NULL), mSpeed(1.0f){
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
	}
}

void TransformableAnimator::changeState(State state){
	mState = state;

	if(mListener && state != mState){
		mListener->onAnimationStateChanged(this, state);
	}
}

void TransformableAnimator::onProcStart(){
}

void TransformableAnimator::onProcEnd(){
	changeState(eSTATE_FINISHED);
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
	else{
		mCurrentTime = mAnimation->getDuration();
		finished = true;
	}

	glm::mat4 mat;
	mAnimation->evaluate(mCurrentTime, mat);

	mTarget->setTransformMatrix(mat);

	if(finished){
		changeState( eSTATE_FINISHED );
	}
}

} // </wt>