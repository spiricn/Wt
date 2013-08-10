#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"


#include "wt/ParticleEffect.h"
#include "wt/SceneLoader.h"

#include "wt/ScriptProcess.h"
#include "wt/lua/LuaUtils.h"

namespace wt{




class TestDemo : public ADemo{
public:

	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){

		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		getAssets()->load("level1.lua");


		SceneLoader loader(getScene(), getAssets());

		loader.load("scene.lua");

		
		lua::State* state = getManager()->getLuaState();
		
		LuaObject obj = state->box(*getEventManager());
		state->getGlobals().Set("EventManager", obj);

		LuaObject luaCamera = state->box(getScene()->getCamera());
		state->getGlobals().Set("Camera", luaCamera);
	

		LuaObject luaScene = state->box(*getScene());
		state->getGlobals().Set("Scene", luaScene);



		PointLight light;
		light.mColor = Color::green();
		light.mPosition = glm::vec3(156, 5, 235);

		getScene()->addPointLight(light);

		light.mColor = Color::blue();
		light.mPosition = glm::vec3(159, 5, 193);
		light.mDiffuseItensity = 3;

		getScene()->addPointLight(light);

		/*lua::ScriptPtr script1;
		getProcManager().attach(new ScriptProcess(script1 = state->createScript("test_script.lua")));*/

	}

	String getConfigFile() const{
		return "assets/TestDemoConfig.lua";
	}

	String getLevelFile() const{
		return "";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
