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

#include "wt/lua/LuaModule.h"
#include "wt/lua/LuaBindings.h"
#include "wt/TransformableAnimator.h"

namespace wt{

class TestDemo : public ADemo{
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
		if(c == VirtualKey::KEY_y){
			getScene()->clear();
		}
		else{
			ADemo::onKeyDown(c);
		}
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		ModelledActor* actor = (ModelledActor*)getScene()->findActorByName("actor");

		NodeAnimation* node = new NodeAnimation;

		NodeAnimation::PositionKey posKey;


		glm::vec3 startPos;
		actor->getTransformable()->getTranslation(startPos);


		posKey.value = startPos;
		posKey.time = 0.0f;
		node->getPositionKeys().push_back(posKey);

		posKey.value += glm::vec3(0, 10, 0);
		posKey.time = 3.0f;
		node->getPositionKeys().push_back(posKey);

		posKey.value = startPos;
		posKey.time = 6.0f;
		node->getPositionKeys().push_back(posKey);

		NodeAnimation::RotationKey rotKey;

		rotKey.time = 0.0f;
		rotKey.value = glm::angleAxis(0.0f, glm::vec3(0, 1, 0));
		node->getRotationKeys().push_back(rotKey);

		rotKey.time = 6.0f;
		rotKey.value = glm::angleAxis(180.0f, glm::vec3(0, 1, 0));
		node->getRotationKeys().push_back(rotKey);

		{
			TransformableAnimator* animator;

			getProcManager().attach( animator = new TransformableAnimator(actor->getController(), node, true) );

			animator->setSpeed(5.0f);
		}

		{
			Animation* anim = new Animation;
			getAssets()->getAnimationManager()->getLoader()->load("c:/Users/Nikola/Desktop/camera.wta", anim);
			getProcManager().attach(  new TransformableAnimator(&getScene()->getCamera(), anim, "default", true) );
		}
	}

	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
