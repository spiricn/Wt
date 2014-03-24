#include "wt/stdafx.h"
#include "wt/AnimatorProcess.h"

#define TD_TRACE_TAG "AnimatorProcess"

namespace wt
{

AnimatorProcess::AnimatorProcess(ModelledActor* actor, const String& animation, float speed, bool blend, float blendSpeed) : mActor(actor), 
	mAnimation(animation), mSpeed(speed), mBlendSpeed(blendSpeed), mBlend(blend){
}

void AnimatorProcess::onProcStart(){
	if(mBlend){
		mActor->blendToAnimation(mAnimation, mBlendSpeed, false)->setSpeed( mSpeed );
	}
	else{
		mActor->getAnimationPlayer()->play(mAnimation, false);
		mActor->getAnimationPlayer()->setSpeed(mSpeed);
	}
}

void AnimatorProcess::onProcUpdate(float dt){
	if(!mActor->isAnimated()){
		killProcess();
	}
}
	
} // </wt>

