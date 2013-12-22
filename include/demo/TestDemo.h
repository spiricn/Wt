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
		glm::vec3 eyePos, fw;
		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(eyePos);

		/*getAssets()->getSoundSystem()->setListenerForwardVec(fw);
		getAssets()->getSoundSystem()->setListenerPosition(eyePos);*/

		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		

		PbSceneLoader loader(getAssets(), getScene());

		loader.save("test.wtpbs");

		getScene()->clear();

		loader.load("test.wtpbs");
	}

	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
