#ifndef WT_ANIMATIONDEMO_H
#define WT_ANIMATIONDEMO_H

#include "demo/ADemo.h"

#include <wt/CameraAnimation.h>
#include <wt/EventTable.h>

#define RECORDER_ENABLED 0

namespace wt{




class AnimationDemo : public ADemo{
private:
	SceneActor* mActor;
	CameraAnimation mCameraAnimation;
	EventTable<AnimationDemo> mEventTable;
	Gui::ProgressView* mAniProgress;
#if RECORDER_ENABLED
	CameraAnimationBuilder mb;
#endif

public:

	AnimationDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		mCameraAnimation.update(dt);

		mAniProgress->setProgress( mCameraAnimation.getProgress()*100.0f );
	}

	void onMouseDown(float x, float y, MouseButton btn){
#if RECORDER_ENABLED
		if(btn == BTN_RIGHT){
			static bool fz=false;

			if(fz){
				mb.addKeyframe();
				LOG("ADD");
			}

			else{
				LOG("START");
				fz = true;
				mb.start(&getScene()->getCamera(), "ani_demo_1");
			}
		}
		else if(btn == BTN_MIDDLE){
			mb.stop();
			mb.saveAnimation("camera_animation.wta");
			LOG("DONE");
		}
#endif
	}


	void onStart(const LuaObject& config){
		mEventTable.init(this, getEventManager());

		getCameraControl()->setCamera(&getScene()->getCamera());

		// Create scene actor
		mActor = getScene()->createActor();
		mActor->setModel(getAssets()->getModelManager()->find("character"), "default");

		mActor->getAnimationPlayer()->play("stand", true);

		mActor->getTransform().setPosition(200, 0, 200);
		mActor->getTransform().setScale(10, 10, 10);

		// Load camera animation
		Animation* ani = getAssets()->getAnimationManager()->create("ani_demo_animation");
		getAssets()->getAnimationManager()->getLoader()->load("assets/demo/AnimationDemo/camera_animation.wta", ani);

		setupGUI();
	}

	void onRenderBonesToggle(){
		getRenderer()->setRenderBones( ((Gui::Checkbox*)getUi().findView("renderBonesCb"))->isChecked() );
	}

	bool onPlayAnimation(const Sp<Event>){
		mCameraAnimation = CameraAnimation(  getAssets()->getAnimationManager()->find("ani_demo_animation"), &getScene()->getCamera());
		return false;
	}

	bool onNextAnimation(const Sp<Event>){
		if(mActor->getAnimationPlayer()->getCurrentAnimation()){
			String currentAnimation = mActor->getAnimationPlayer()->getCurrentAnimation()->getAnimation()->getName();

			// Search for a next available character animation
			for(Model::AnimationMap::iterator i=mActor->getModel()->getAnimations().begin(); i!=mActor->getModel()->getAnimations().end(); i++){
				if(i->first.compare(currentAnimation) == 0){
					Model::AnimationMap::iterator next = i;
					next++;
					next = next == mActor->getModel()->getAnimations().end() ? mActor->getModel()->getAnimations().begin() : next;
					mActor->getAnimationPlayer()->play(next->first, true);
				}
			}
		}
		else{
			mActor->getAnimationPlayer()->play(
				mActor->getModel()->getAnimations().begin()->second, true);
		}


		return false;
	}

	String getConfigFile() const{
		return "assets/AnimationDemoConfig.lua";
	}

	String getLevelFile() const{
		return "assets/demo/AnimationDemo/level.lua";
		//return "world.lua";
	}


	void onAnimationSpeedChanged(){
		
		float factor = ((Gui::SliderView*)getUi().findView("animSpeedSlider"))->getValue()/100.0f;

		mActor->getAnimationPlayer()->setSpeed( factor );
	}

	void setupGUI(){
		getUi().setGridSize(20, 10);
		getUi().setDefaultScaleMode(Gui::View::eGRID);

		{
			// Animation toggle button
			Gui::Button* v = getUi().createView<Gui::Button>("btnNextAnim");

			v->setText("Next animation");
			v->setGridLocation(19, 0, 1, 2);

			mEventTable.registerCallback(Gui::ButtonClickedEvent::TYPE,
				&AnimationDemo::onNextAnimation, v->getId());
		}

		{
			// Camera animation playback button
			Gui::Button* v = getUi().createView<Gui::Button>();

			v->setText("Play animation");
			v->setGridLocation(19, 2, 1, 2);

			mEventTable.registerCallback(Gui::ButtonClickedEvent::TYPE,
				&AnimationDemo::onPlayAnimation, v->getId());
		}

		{
			// Camera animation progress view
			mAniProgress = getUi().createView<Gui::ProgressView>();

			mAniProgress->setGridLocation(19, 4, 1, 2);
		}

		{
			// Render bones toggle checkbox
			Gui::Checkbox* v = getUi().createView<Gui::Checkbox>("renderBonesCb");
			v->setText("Show bones");
			v->setGridLocation(19, 6, 1, 2);

			mEventTable.registerCallback(Gui::CheckboxClickedEvent::TYPE,
				&AnimationDemo::onRenderBonesToggle, v->getId());
		}

		{
			// Animation speed slider
			Gui::SliderView* v = getUi().createView<Gui::SliderView>("animSpeedSlider");

			v->setGridLocation(18, 0, 1, 2);

			mEventTable.registerCallback(Gui::SliderValueChangedEvent::TYPE,
				&AnimationDemo::onAnimationSpeedChanged, v->getId());
		}

		{
			// Camera animation playback button
			Gui::Button* v = getUi().createView<Gui::Button>();

			v->setText("Toggle model");
			v->setGridLocation(18, 2, 1, 2);

			mEventTable.registerCallback(Gui::ButtonClickedEvent::TYPE,
				&AnimationDemo::onToggleModel, v->getId());
		}
	}

	void onToggleModel(){
		static const int maxModels = 5;
		static const char* models[maxModels] = {"character", "ghoul", "wolf", "guard", "tarantula"};
		static int currModel = 0;

		currModel = (currModel+1)%maxModels;

		mActor->setModel(
			getAssets()->getModelManager()->find(models[currModel]),
			getAssets()->getModelManager()->find(models[currModel])->getSkins().begin()->second->getName()
			);

		onNextAnimation(0);
	}

}; // </AnimationDemo>

WT_DECLARE_DEMO(AnimationDemo)

}; // </wt>

#endif // </WT_ANIMATIONDEMO_H>
