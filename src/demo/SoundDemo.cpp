#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/MusicPlayer.h>
#include <wt/TransformableAnimator.h>


using namespace wt;

#define TD_TRACE_TAG "SoundDemo"

class SoundDemo : public ADemo{
public:

	SoundDemo(){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt);

		glm::vec3 fw, pos;

		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(pos);

		getAssets()->getSoundSystem()->setListenerForwardVec( fw );
		getAssets()->getSoundSystem()->setListenerPosition( pos);
	}
	
	void onKeyUp(wt::VirtualKey code){
		ADemo::onKeyUp(code);
	}

	void setupGui(){
		getInput()->setMouseGrabbed(false);

		mUi = getScene()->getUIWindow();

		// Setup GUI
		mUi->setDefaultScaleMode( gui::View::eSCALE_MODE_GRID );
		mUi->setGridSize(20, 20);

		
		// Next track toggle button
		{
			gui::Button* btn = mUi->createView<gui::Button>("btn_next");
			btn->setGridLocation(19, 0, 1, 2);
			btn->setText("Next track");

			/*getEventManager()->registerCallback(
				new MemberCallback<SoundDemo>(this, &SoundDemo::onNextClicked), gui::ButtonClickedEvent::TYPE, true, btn->getId()
				);*/
			TRACEW("TODO");
		}

		// Current track display
		{
			gui::TextView* view = mUi->createView<gui::TextView>("current_track");
			
			view->setText("");
			view->setGridLocation(19, 3, 1, 2);
		}

		// Music volume slider
		{
			
			gui::SliderView* v = getScene()->getUIWindow()->createView<gui::SliderView>("volume_slider");
			
			v->setGridLocation(19, 6, 1, 3);

			/*getEventManager()->registerCallback(
				new MemberCallback<SoundDemo>(this, &SoundDemo::onVolumeChanged), gui::SliderValueChangedEvent::TYPE, true, v->getId()
				);*/
			TRACEW("TODO");
		}
	}
	
	void onDemoStart(const LuaObject& config){
		// Create a music playlist
		Playlist pl;

		pl.addTrack("d:/Documents/prog/c++/workspace/Wt/assets/music/dayforest01.ogg");
		pl.addTrack("d:/Documents/prog/c++/workspace/Wt/assets/music/dayforest02.ogg");
		pl.addTrack("d:/Documents/prog/c++/workspace/Wt/assets/music/dayforest03.ogg");

		// Create a music player
		getProcessManager()->attach( mMusic = new MusicPlayer( getAssets()->getSoundSystem() ));

		mMusic->setPlayList(pl);
		mMusic->setVolume(0.1f);

		// Start playlig
		mMusic->play();

		// Create GUI
		setupGui();


		// Start camera animation
		getProcessManager()->attach
			(new TransformableAnimator(&getScene()->getCamera(),
			getAssets()->getAnimationManager()->getFromPath("$ROOT/sound_demo/camera"), "default", true)
			);

	}

	void onVolumeChanged(){
		// Voume slider value changed, update volume accordingly
		mMusic->setVolume( dynamic_cast<gui::SliderView*>(mUi->findView("volume_slider"))->getValue() / 100.0f );
	}

	void onNextClicked(){
		// Play next song in the playlist
		mMusic->play();

		dynamic_cast<gui::TextView*>(mUi->findView("current_track"))->setText(
			utils::getPathBaseName( mMusic->getPlaylist().getCurrent() ));
	}

	String getScriptPath() const{
		return "demo_workspace/SoundDemo/main.lua";
	}

	MusicPlayer* mMusic;
	gui::Window* mUi;
}; // </SoundDemo>

WT_DECLARE_DEMO_IMPL(SoundDemo);
