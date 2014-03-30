#include "wt/stdafx.h"
#include "wt/AnimatorProcess.h"

#define TD_TRACE_TAG "AnimatorProcess"

namespace wt
{


AnimatorProcess::AnimatorProcess(ModelledActor* actor, const String& animation, float speed, bool blend, float blendSpeed) : mActor(actor),
	AProcess(utils::print("AnimatorProcess-%p", static_cast<void*>(this))), mFirstAnimation(true){
	if(animation.size()){
		addAnimation(animation, speed, blend, blendSpeed);
	}
}

AnimatorProcess* AnimatorProcess::addAnimation(const String& name , float speed, bool blend, float blendSpeed, bool loop, float loopDuration){
	AnimationEntry e;
	e.blend = blend;
	e.blendSpeed = blendSpeed;
	e.speed = speed;
	e.name = name;
	e.loop = loop;
	e.loopDuration = loopDuration;
	mEntries.push_back(e);

	return this;
}


void AnimatorProcess::onProcStart(){
}

void AnimatorProcess::onProcUpdate(float dt){
	AnimationEntry* currEntry = NULL;

	if(!mEntries.empty()){
		currEntry = &mEntries.front();
	}

	if(!mActor->isAnimated() || mFirstAnimation || (currEntry && currEntry->loop && currEntry->loopDuration <= 0.0f)){
		// Proceed to next entry

		if(currEntry && !mFirstAnimation){
			// Remove the previous one
			mEntries.erase(mEntries.begin());
		}

		if(mEntries.empty()){
			// All entries processed
			killProcess();
			return;
		}
		else{
			currEntry = &mEntries.front();
		}

		mFirstAnimation = false;

		if(currEntry->blend){
			mActor->blendToAnimation(currEntry->name, currEntry->blendSpeed, currEntry->loop)->setSpeed( currEntry->speed );
		}
		else{
			mActor->getAnimationPlayer()->play(currEntry->name, currEntry->loop);
			mActor->getAnimationPlayer()->setSpeed(currEntry->speed);
		}
	}
	else if(currEntry && currEntry->loop){
		currEntry->loopDuration -= dt;
	}
}
	
} // </wt>

