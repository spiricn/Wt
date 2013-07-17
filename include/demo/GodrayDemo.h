#ifndef WT_GODRAY_H
#define WT_GODRAY_H

#include "demo/ADemo.h"

namespace wt{

class GodRayDemo : public ADemo{
public:
	void onRender(float dt){
		
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);

		getCameraControl()->handle(
			dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		if(c == KEY_f){
			stopDemo();
		}

		ADemo::onKeyDown(c);
	}

	String getLevelFile() const{
		return "world.lua";
	}

	String getConfigFile() const{
		return "assets/GodRayDemoConfig.lua";
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		//Renderer::GodRayParams p;
		//getRenderer()->getGodRayParams(p);
		//p.mSunPos = glm::vec3(-0.385030, 0.453779, -0.803640)*1000.0f;
		//p.mIsEnabled = false;
		//p.mSunSize = 30.0f;

		//p.mIsEnabled = true;

		//getRenderer()->setGodRayParams(p);
	}
};

WT_DECLARE_DEMO(GodRayDemo)

}; // </wt>

#endif // </WT_GODRAY_H>