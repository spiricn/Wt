#include "demo/stdafx.h"

#include "demo/ADemo.h"

using namespace wt;

#define TD_TRACE_TAG "FogDemo"

using namespace wt;

class FogDemo : public ADemo{
public:
	FogDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void printHelp(){
		LOGI("Hold I to increase fog density");

		LOGI("Hold O to decrease fog density");
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt, getInput());

		FogDesc fog = getScene()->getFogDesc();

		if(getInput()->isKeyDown(KEY_i)){
			fog.density += 0.01f * dt;
		}
		else if(getInput()->isKeyDown(KEY_o)){
			fog.density -= 0.01f * dt;
		}

		getScene()->setFogDesc(fog);
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());
	}

	String getScriptPath() const{
		return "demo_workspace/FogDemo/main.lua";
	}

}; // </FogDemo>

WT_DECLARE_DEMO_IMPL(FogDemo);
