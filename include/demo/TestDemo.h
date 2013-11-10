#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"

#include "wt/ParticleEffect.h"
#include "wt/SceneLoader.h"
#include "wt/lua/LuaActor.h"
#include "wt/ScriptProcess.h"
#include "wt/lua/LuaUtils.h"
#include "wt/IcosphereBuilder.h"
#include "wt/WaitProcess.h"
#include "wt/lua/LuaModule.h"
#include "wt/lua/LuaBindings.h"
#include "wt/TransformableAnimator.h"

#define ANIM_DIR "d:/Documents/prog/c++/workspace/Wt/workspace/godray"

#define ANIMATION_SPEED ( 0.6f )
namespace wt
{


class AnimationProcess : public AProcess{
public:
	AnimationProcess(ModelledActor* actor, const String& animation, float speed, bool blend=true, float blendSpeed=0.1f) : mActor(actor), 
		mAnimation(animation), mSpeed(speed), mBlendSpeed(blendSpeed), mBlend(blend){
	}

	void onProcStart(){
		if(mBlend){
			mActor->blendToAnimation(mAnimation, mBlendSpeed, false)->setSpeed( mSpeed );
		}
		else{
			mActor->getAnimationPlayer()->play(mAnimation, false);
			mActor->getAnimationPlayer()->setSpeed(mSpeed);
		}
	}

	void onProcUpdate(float dt){
		if(!mActor->isAnimated()){
			killProcess();
		}
	}

private:
	ModelledActor* mActor;
	String mAnimation;
	float mSpeed;
	bool mBlend;
	float mBlendSpeed;
};

class SceneProc : public AProcess{
public:
	enum Stage{
		eSTAGE_0,
		eSTAGE_1,
		eSTAGE_2,
		eSTAGE_3,
		eSTAGE_4,
		eSTAGE_TURN_ON_LIGHTS
	};

	SceneProc(ADemo* demo, Stage stage=eSTAGE_0) : mDemo(demo), mStage(stage){
	}

	void onProcUpdate(float dt){
		ModelledActor* mage = (ModelledActor*)mDemo->getScene()->findActorByName("mage");

		if(mStage == eSTAGE_3){
			 const glm::vec3 endPos = glm::vec3(287.347748, 0.100000, 254.291016);
			 glm::vec3 currPos;

			 mage->getTransformable()->getTranslation(currPos);

			 if(glm::length(endPos-currPos) <= 1.5f){
				 setNext(new SceneProc(mDemo, eSTAGE_4));
				 mage->blendToAnimation("stand1", 0.5f, false)->setSpeed(ANIMATION_SPEED);
				 killProcess();
			 }

			 glm::vec3 moveVec = glm::normalize(endPos-currPos);

			 const float moveSpeed = 9.0f;

			 currPos += moveVec*moveSpeed*dt;
			 mage->getController()->setTranslation(currPos);
		}
		else if(mStage == eSTAGE_TURN_ON_LIGHTS){
			float val = 0.0f;
			if(((Interpolator<float>*)mStageData)->update(dt, val)){
				delete ((Interpolator<float>*)mStageData);
				killProcess();
			}
			else{
				LOG("TURNING ON THE FUCKING LIGHTS %f", val);
				const PointLight* light = (PointLight*)mDemo->getScene()->findActorByName("mage_light");
				PointLight::Desc desc = light->getDesc();
				desc.diffuseIntensity = val;
				light->setDesc(desc);
			}
		}
	}

	void onProcStart(){
		LOGI("Stage %d started.", mStage);

		ModelledActor* mage = (ModelledActor*)mDemo->getScene()->findActorByName("mage");

		if(mStage == eSTAGE_TURN_ON_LIGHTS){
			mStageData = new Interpolator<float>(0, 28, 1.5f, false, Interpolator<float>::eEASE_IN_QUAD);
		}
		else if(mStage == eSTAGE_0){
			Animation* anim = new Animation;
			mDemo->getAssets()->getAnimationManager()->getLoader()->load(ANIM_DIR "/camera.wta", anim);

			TransformableAnimator* animator;
			this
				->setNext( new WaitProcess(3.0f) )
				->setNext( animator = new TransformableAnimator(&mDemo->getScene()->getCamera(), anim, "default", false, true) )
				->setNext( new WaitProcess(2.0f) )
				->setNext( new SceneProc(mDemo, eSTAGE_TURN_ON_LIGHTS) )
				->setNext(new SceneProc(mDemo, eSTAGE_1));

			//animator->setSpeed(50);

			killProcess();
		}
		else if(mStage == eSTAGE_1){
			mage->getAnimationPlayer()->play("kneel_end", false);
			mage->getAnimationPlayer()->setSpeed(0.6);
		

			Animation* anim = new Animation;
			mDemo->getAssets()->getAnimationManager()->getLoader()->load(ANIM_DIR "/camera2.wta", anim);

			TransformableAnimator* animator1 = new TransformableAnimator(&mDemo->getScene()->getCamera(), anim, "default", false, true);

			this
				->setNext(animator1)
				->setNext( new AnimationProcess(mage, "stand3", 0.6) )
				->setNext( new SceneProc(mDemo, eSTAGE_2)  );

			killProcess();
		}
		else if(mStage == eSTAGE_2){

			Animation* cameraAnimation = new Animation;
			mDemo->getAssets()->getAnimationManager()->getLoader()->load(ANIM_DIR "/camera3.wta", cameraAnimation);
			TransformableAnimator* cameraAnimator = new TransformableAnimator(&mDemo->getScene()->getCamera(), cameraAnimation, "default", false, true);
			mDemo->getProcManager().attach(cameraAnimator );

			
			this
				//->setNext(new AnimationProcess(mage, "stand3", 0.6))
				->setNext(new SceneProc(mDemo, eSTAGE_3));
			killProcess();
		}
		else if(mStage == eSTAGE_3){
			//mage->getAnimationPlayer()->play("walk", true);
			mage->blendToAnimation("walk", 0.3f, true)->setSpeed( ANIMATION_SPEED );

			mage->getAnimationPlayer()->setSpeed(0.6f);
		}
		else if(mStage == eSTAGE_4){
			Animation* anim = new Animation;
			mDemo->getAssets()->getAnimationManager()->getLoader()->load(ANIM_DIR "/camera4.wta", anim);

			TransformableAnimator* animator1 = new TransformableAnimator(&mDemo->getScene()->getCamera(), anim, "default", false, true);
			setNext(animator1);
			killProcess();
		}
	}

	void onProcEnd(){
	}

private:
	ADemo* mDemo;
	Stage mStage;
	void* mStageData;
}; 

class TestDemo : public ADemo, public TransformableAnimator::IListener{
public:

	ASceneActor* mMainActor;

	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		glm::vec3 eyePos, fw;
		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(eyePos);

		getAssets()->getSoundSystem()->setListenerForwardVec(fw);
		getAssets()->getSoundSystem()->setListenerPosition(eyePos);

		getPhysics()->update(dt);
		getScene()->update(dt);
		//getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		if(c == KEY_y){
			getScene()->clear();
		}
		else if(c == KEY_n){
			static const int numAnims = 2;
			static const char* anims[numAnims] = {"kneel_loop", "kneel_end"};
			static int currAnim = 0;

			currAnim = (currAnim + 1) % numAnims;

			LOG("Plaing animation: %s", anims[currAnim]);

			ModelledActor* actor = (ModelledActor*)getScene()->findActorByName("actor");
			//actor->getAnimationPlayer()->play(anims[currAnim], true);
			//actor->addAnimationChannel()->play(anims[currAnim], true);
			//actor->blendToAnimation(anims[currAnim], .5f);
		}
		else{
			ADemo::onKeyDown(c);
		}
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		getProcManager().attach(new SceneProc(this) );
				
	}

	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
