#ifndef WT_GAMEPROC_H
#define WT_GAMEPROC_H

#include "wt/ProcessManager.h"

namespace wt{


#if 0

class Unit{
public:
	float mMoveSpeed;
	DynamicActor* mActor;

};

class Animator : public AProcess{
private:
	SceneActor* mActor;
	String mAnimation;
	bool mLoop, mWait;

public:

	Animator(SceneActor* actor, const String& animation, bool loop, bool wait=true) : mActor(actor), mAnimation(animation), mLoop(loop), mWait(wait){
	}

	void onProcUpdate(float dt){
	/*	if(!mActor->getModel()->isAnimated() || !mWait){
			mActor->getModel()->setActiveAnimation(mAnimation, mLoop);
			killProcess();
		}*/
	}

};

class SoundPlayer : public AProcess{
private:
	Sp<ASound> mSound;

public:
	~SoundPlayer(){
	}

	SoundPlayer(const String& path){
		mSound = TheGame.sound->createSound(path);
		mSound->setRelativeToListener(true);
	}

	void onProcStart(){
		mSound->play();
	}

	void onProcUpdate(float dt){
		if(mSound->getStatus() != ASound::ePLAYING){
			killProcess();
		}
	}
};

class ProcDelayer : public AProcess{
private:
	ProcPtr mTarget;
	float mDelayTime, mTime;

public:
	ProcDelayer(ProcPtr target, float delayTime) : mTarget(target), mDelayTime(delayTime), mTime(0.0f){
	}

	void onProcUpdate(float dt){
		mTime += dt;
		if(mTime >= mDelayTime){
			getManager()->attach(mTarget);
			killProcess();
		}
	}
};

class CameraCtrlProc : public AProcess, public EventListener{
private:
	float mMoveSpeed;
	float mMouseSensitivity;

	void moveForward(float dt){
		TheGame.scene->getCamera().move( glm::vec3(0.0f, 0.0f, 1.0f) * (mMoveSpeed*dt) );
	}

	void moveBackward(float dt){
		TheGame.scene->getCamera().move( glm::vec3(0.0f, 0.0f, -1.0f) * (mMoveSpeed*dt) );
	}

	void strifeLeft(float dt){
		TheGame.scene->getCamera().move( glm::vec3(1.0f, 0.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void strifeRight(float dt){
		TheGame.scene->getCamera().move( glm::vec3(-1.0f, 0.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void moveUp(float dt){
		TheGame.scene->getCamera().move( glm::vec3(0.0f, 1.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void moveDown(float dt){
		TheGame.scene->getCamera().move( glm::vec3(0.0f, -1.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void lookHorizontal(float dx){
		TheGame.scene->getCamera().yaw(-dx*mMouseSensitivity);
	}

	void lookVertical(float dy){
		TheGame.scene->getCamera().pitch(dy*mMouseSensitivity);
	}
public:
	CameraCtrlProc() : mMoveSpeed(50.0f), mMouseSensitivity(2.0f){
	}

	 bool handleEvent(const Sp<Event> e){
		 EvtType type = e->getType();

		 return true;
	 }

	void onProcUpdate(float dt){
		if(*TheGame.freeFlyMode){
			if(TheGame.input->isKeyDown(KEY_w)){
				moveForward(dt);
			}
			else if(TheGame.input->isKeyDown(KEY_s)){
				moveBackward(dt);
			}

			if(TheGame.input->isKeyDown(KEY_a)){
				strifeLeft(dt);
			}
			else if(TheGame.input->isKeyDown(KEY_d)){
				strifeRight(dt);
			}

			if(TheGame.input->isKeyDown(KEY_SPACE)){
				moveUp(dt);
			}
			else if(TheGame.input->isKeyDown(KEY_c)){
				moveDown(dt);
			}

			if(TheGame.input->isMouseGrabbed()){
				lookHorizontal(TheGame.input->getMouseDeltaX());
				lookVertical(TheGame.input->getMouseDeltaY());
			}
		}
		else{
		}
	}
};

#endif

}; // </Wt>

#endif