#ifndef WT_ANIMATIONDEMO_H
#define WT_ANIMATIONDEMO_H

#include "demo/ADemo.h"

#include <wt/EventTable.h>

#define RECORDER_ENABLED 0

namespace wt{




class AnimationDemo : public ADemo{
private:
	ModelledActor* mActor;
	//CameraAnimation mCameraAnimation;
	gui::ProgressView* mAniProgress;
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

		/*mCameraAnimation.update(dt);

		mAniProgress->setProgress( mCameraAnimation.getProgress()*100.0f );*/
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
		//// Create scene actor
		mActor = dynamic_cast<ModelledActor*>( getScene()->findActorByName("actor") );

		setupGUI();
	}

	void onRenderBonesToggle(){
		getRenderer()->setRenderBones( ((gui::Checkbox*)getUi().findView("renderBonesCb"))->isChecked() );
	}

	void onPlayAnimation(){
		//mCameraAnimation = CameraAnimation(  getAssets()->getAnimationManager()->find("ani_demo_animation"), &getScene()->getCamera());
	}

	void onNextAnimation(){
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

	}
		
	String getScriptPath() const{
		return "workspace/demos/animation_demo/main.lua";
	}

	void onAnimationSpeedChanged(){
		
		float factor = ((gui::SliderView*)getUi().findView("animSpeedSlider"))->getValue()/100.0f;

		mActor->getAnimationPlayer()->setSpeed( factor * 2 );
	}

	void setupGUI(){
		getUi().setGridSize(20, 10);
		getUi().setDefaultScaleMode(gui::View::eGRID);

		{
			// Animation toggle button
			gui::Button* v = getUi().createView<gui::Button>("btnNextAnim");

			v->setText("Next animation");
			v->setGridLocation(19, 0, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<AnimationDemo>(this, &AnimationDemo::onNextAnimation), gui::ButtonClickedEvent::TYPE, true, v->getId()
				);
		}

		{
			// Camera animation playback button
			gui::Button* v = getUi().createView<gui::Button>();

			v->setText("Play animation");
			v->setGridLocation(19, 2, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<AnimationDemo>(this, &AnimationDemo::onPlayAnimation), gui::ButtonClickedEvent::TYPE, true, v->getId()
				);
		}

		{
			// Camera animation progress view
			mAniProgress = getUi().createView<gui::ProgressView>();

			mAniProgress->setGridLocation(19, 4, 1, 2);
		}

		{
			// Render bones toggle checkbox
			gui::Checkbox* v = getUi().createView<gui::Checkbox>("renderBonesCb");
			v->setText("Show bones");
			v->setGridLocation(19, 6, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<AnimationDemo>(this, &AnimationDemo::onRenderBonesToggle), gui::CheckboxClickedEvent::TYPE, true, v->getId()
			);
		}

		{
			// Animation speed slider
			gui::SliderView* v = getUi().createView<gui::SliderView>("animSpeedSlider");

			v->setGridLocation(18, 0, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<AnimationDemo>(this, &AnimationDemo::onAnimationSpeedChanged), gui::SliderValueChangedEvent::TYPE, true, v->getId()
			);
		}

		{
			// Camera animation playback button
			gui::Button* v = getUi().createView<gui::Button>();

			v->setText("Toggle model");
			v->setGridLocation(18, 2, 1, 2);

			getEventManager()->registerCallback(
				new MemberCallback<AnimationDemo>(this, &AnimationDemo::onToggleModel), gui::ButtonClickedEvent::TYPE, true, v->getId()
			);
		}
	}

	void onToggleModel(){
		static const int maxModels = 1;
		static const char* models[maxModels] = {"mage"};
		static int currModel = 0;

		currModel = (currModel+1)%maxModels;

		mActor->setModel(
			getAssets()->getModelManager()->find(models[currModel]),
			getAssets()->getModelManager()->find(models[currModel])->getSkins().begin()->second->getName()
			);

		onNextAnimation();
	}

}; // </AnimationDemo>

WT_DECLARE_DEMO(AnimationDemo)

}; // </wt>

#endif // </WT_ANIMATIONDEMO_H>
