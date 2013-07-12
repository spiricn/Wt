#ifndef WT_SOUNDDEMO_H
#define WT_SOUNDDEMO_H

#include "demo/ADemo.h"

#include <wt/ASoundSystem.h>

namespace wt{

class SoundDemo : public ADemo{
private:
	SoundPtr mSound;
	ASoundSystem* mSoundSystem;
	SoundStreamPtr mMusic;
	static const char* TAG;
public:
	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		mSoundSystem->setListenerForwardVec( getScene()->getCamera().getForwardVec() );
		mSoundSystem->setListenerPosition( getScene()->getCamera().getPosition() );
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);

		if(c == KEY_m){
			// Play music
			if(mMusic->getStatus() == ASoundStream::ePLAYING){
				LOGI(TAG,
					"Stopping music");
				mMusic->stop();
			}
			else{
				mMusic->play();
				LOGI(TAG,
					"Playing music");
			}
		}
		else if(c == 269){
			// Volume down
			mSoundSystem->setGlobalVolume( glm::clamp(mSoundSystem->getGlobalVolume() - 0.1f, 0.0f, 1.0f) );
		}
		else if(c == 270){
			// Volume up
			mSoundSystem->setGlobalVolume( glm::clamp(mSoundSystem->getGlobalVolume() + 0.1f, 0.0f, 1.0f) );
		}
	}

	void printHelp(){
		LOGI(TAG,
			"Press M to toggle music");

		LOGI(TAG,
			"Press +/- to change global volume");

		LOGI(TAG,
			"Press H for help");
	}


	String getConfigFile() const{
		return "assets/SoundDemoConfig.lua";
	}

	String getLevelFile() const{
		return "world.lua";
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		// Create scene actor
		SceneActor* actor = getScene()->createActor();

		actor->getTransform().setPosition(glm::vec3(195.235229, -3.291626, 216.325455));

		actor->setModel(
			getAssets()->getModelManager()->getFromPath("$ROOT/wolf"), "timber");

		actor->getAnimationPlayer()->play("attack", true);

		// Create sound 
		mSoundSystem = new SFSoundSystem((SFSoundManager*)getAssets()->getSoundManager());

		mSound = mSoundSystem->createSound( getAssets()->getSoundManager()->getFromPath("$ROOT/wolf/attack") );

		mSound->setPosition( actor->getTransform().getPosition() );
		mSound->setRelativeToListener(false);

		mSound->setLoop(true);
		mSound->play();

		// Create music
		mMusic = mSoundSystem->createSoundStream("assets/music/dayforest03.ogg");
		mMusic->setLoop(true);
		mMusic->play();
	}

}; // </SoundDemo>

WT_DECLARE_DEMO(SoundDemo)


const char* SoundDemo::TAG = "SoundDemo";

}; // </wt>

#endif // </WT_SOUNDDEMO_H>
