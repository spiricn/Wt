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
#include "wt/PbSceneLoader.h"

namespace wt
{

class TestDemo : public ADemo, public TransformableAnimator::IListener{
public:

	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){

		/*getScene()->getCamera().setTranslation(glm::vec3(206.809372, 95.402977, 239.410477));
		getScene()->getCamera().setOrientation(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));*/
		glm::vec3 eyePos, fw;
		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(eyePos);


		/*getAssets()->getSoundSystem()->setListenerForwardVec(fw);
		getAssets()->getSoundSystem()->setListenerPosition(eyePos);*/


		static FloatInterpolator inter(0, 360, 5, true);
		float d;
		inter.update(dt, d);

		ModelledActor* a = dynamic_cast<ModelledActor*>(getScene()->findActorByName("cube"));
		a->getTransformable()->setRotation(glm::normalize(glm::vec3(1, 1, 1)), d);

		/*static glm::vec3 startPos;
		glm::vec3 pos;
		static bool flag = false;
		if(!flag){
			a->getTransformable()->getTranslation(startPos);
			flag = true;
		}
		
		pos = startPos + glm::vec3(0, 0, d);
		a->getTransformable()->setTranslation(pos);*/


		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		// Enable shadow mapping
		Scene::ShadowMappingDesc desc = getScene()->getShadowMappingDesc();
		desc.enabled = true;
		desc.casterSource.setTranslation(glm::vec3(206.809372, 95.402977, 239.410477));
		desc.casterSource.setOrientation(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
		getScene()->setShadowMappingDesc(desc);

/*
		

		PbSceneLoader loader(getAssets(), getScene());

		loader.save("test.wtpbs");

		getScene()->clear();

		loader.load("test.wtpbs");*/
	}

	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
