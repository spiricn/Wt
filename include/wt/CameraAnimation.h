#ifndef WT_CAMERAANIMATION_H
#define WT_CAMERAANIMATION_H

#include "wt/Animation.h"
#include "wt/Camera.h"
#include "wt/AnimationLoader.h"
#include "wt/Timer.h"

namespace wt{

class CameraAnimation{
private:
	Animation* mAnimation;
	float mTime;
	math::Camera* mCamera;

	bool mDone;
public:
	

	CameraAnimation(Animation* animation=NULL, math::Camera* camera=NULL) : mTime(0.0f),
		mAnimation(animation), mCamera(camera), mDone(false){
	}

	float getProgress() const{
		if(mAnimation){
			return mTime/mAnimation->getDuration();
		}
		else{
			return 0.0f;
		}
	}


	void update(float dt){
		if(!(mAnimation && mCamera && !mDone)){
			return;
		}

		mTime += dt;
		if(mTime >= mAnimation->getDuration()){
			mDone = true;
			mTime = mAnimation->getDuration();
		}
#if 0
		// TODO looping
		mTime = fmod(mTime+dt, mAnimation->getDuration());
#endif
		glm::vec3 pos, scale;
		glm::quat rot;
		mAnimation->mNodeAnimations[0]->evaluate(mTime, pos, rot, scale);
		mCamera->setPosition(pos);
		mCamera->setRotation(rot);
	}

}; // </CameraAnimation>

class CameraAnimationBuilder{
private:
	math::Camera* mCamera;
	Timer mTimer;
	NodeAnimation* mAnimationSink;
	Animation mAnimation;

public:
	CameraAnimationBuilder() : mCamera(NULL){
	}

	void start(math::Camera* target, const String& channelName){
		mCamera = target;
		mTimer.reset();
		mAnimationSink = mAnimation.addNodeAnimation();
		mAnimationSink->setTargetNode(channelName);

		addKeyframe();
	}

	void addKeyframe(){
		glm::vec3 pos = mCamera->getPosition();

		glm::quat rot;
		mCamera->getRotation(rot);

		float time = mTimer.peekSeconds();

		mAnimationSink->getPositionKeys().push_back(
			NodeAnimation::PositionKey(pos.x, pos.y, pos.z, time)
			);

		mAnimationSink->getRotationKeys().push_back(
			NodeAnimation::RotationKey(rot.x, rot.y, rot.z, rot.w, time)
			);
	}

	void stop(){
		// TODO use node duration, not the animation duration
		mAnimation.setDuration(mTimer.peekSeconds());
	}

	void saveAnimation(const String& path){
		FileIOStream stream(path, AIOStream::eMODE_WRITE);
		AnimationLoader::getSingleton().save(&stream, &mAnimation);
	}

}; // </CameraAnimationBuilder>


}; // </wt>

#endif