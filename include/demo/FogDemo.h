#ifndef WT_FOGDEMO_H
#define WT_FOGDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "FogDemo"

namespace wt{

class FogDemo : public ADemo{
public:

	FogDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void printHelp(){
		LOGI(
			"Hold I to increase fog density");

		LOGI(
			"Hold O to decrease fog density");
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());


		FogDesc fog = getScene()->getFogDesc();

		if(getManager()->getInput()->isKeyDown(KEY_i)){
			fog.density += 0.01f * dt;
		}
		else if(getManager()->getInput()->isKeyDown(KEY_o)){
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

	String getConfigFile() const{
		return "assets/FogDemoConfig.lua";
	}

	String getLevelFile() const{
		return "world.lua";
	}

}; // </FogDemo>

WT_DECLARE_DEMO(FogDemo)

}; // </wt>

#endif // </WT_FOGDEMO_H>
