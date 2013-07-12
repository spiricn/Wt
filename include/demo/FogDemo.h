#ifndef WT_FOGDEMO_H
#define WT_FOGDEMO_H

#include "demo/ADemo.h"

namespace wt{

class FogDemo : public ADemo{
private:
	static const char* TAG;

public:

	FogDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void printHelp(){
		LOGI(TAG,
			"Hold I to increase fog density");

		LOGI(TAG,
			"Hold O to decrease fog density");
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());


		if(getManager()->getInput()->isKeyDown(KEY_i)){
			getScene()->getFog().density += 0.01f * dt;
		}
		else if(getManager()->getInput()->isKeyDown(KEY_o)){
			getScene()->getFog().density -= 0.01f * dt;
		}
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

const char* FogDemo::TAG = "FogDemo";

}; // </wt>

#endif // </WT_FOGDEMO_H>
