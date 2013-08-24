#ifndef WT_SOUNDDEMO_H
#define WT_SOUNDDEMO_H

#include "demo/ADemo.h"

#include <wt/ASoundSystem.h>

#define TD_TRACE_TAG "SoundDemo"

namespace wt{

class SoundDemo : public ADemo{
private:
	SoundPtr mSound;
	SoundStreamPtr mMusic;

public:
	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		getAssets()->getSoundSystem()->setListenerForwardVec( getScene()->getCamera().getForwardVec() );
		getAssets()->getSoundSystem()->setListenerPosition( getScene()->getCamera().getPosition() );
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);

		if(c == KEY_m){
			// Play music
			if(mMusic->getStatus() == ASoundStream::ePLAYING){
				LOGI(
					"Stopping music");
				mMusic->stop();
			}
			else{
				mMusic->play();
				LOGI(
					"Playing music");
			}
		}
		else if(c == 269){
			// Volume down
			getAssets()->getSoundSystem()->setGlobalVolume( glm::clamp(getAssets()->getSoundSystem()->getGlobalVolume() - 0.1f, 0.0f, 1.0f) );
		}
		else if(c == 270){
			// Volume up
			getAssets()->getSoundSystem()->setGlobalVolume( glm::clamp(getAssets()->getSoundSystem()->getGlobalVolume() + 0.1f, 0.0f, 1.0f) );
		}
	}

	void printHelp(){
		LOGI(
			"Press M to toggle music");

		LOGI(
			"Press +/- to change global volume");

		LOGI(
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
		ModelledActor* actor = getScene()->createModelledActor();

		actor->getTransformable()->setTranslation(glm::vec3(195.235229, -3.291626, 216.325455));

		actor->setModel(
			getAssets()->getModelManager()->getFromPath("$ROOT/wolf"), "timber");

		actor->getAnimationPlayer()->play("attack", true);

		mSound = getAssets()->getSoundSystem()->createSound( getAssets()->getSoundManager()->getFromPath("$ROOT/wolf/attack") );

		glm::vec3 pos;
		actor->getTransformable()->getTranslation(pos);

		mSound->setPosition( pos );
		mSound->setRelativeToListener(false);

		mSound->setLoop(true);
		mSound->play();

		// Create music
		mMusic = getAssets()->getSoundSystem()->createSoundStream("assets/music/dayforest03.ogg");
		mMusic->setLoop(true);
		mMusic->play();
	}

}; // </SoundDemo>

WT_DECLARE_DEMO(SoundDemo)


}; // </wt>

#endif // </WT_SOUNDDEMO_H>
