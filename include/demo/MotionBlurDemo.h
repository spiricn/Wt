#ifndef WT_MOTIONBLURDEMO_H
#define WT_MOTIONBLURDEMO_H

#include "demo/ADemo.h"

#include "wt/EventTable.h"

namespace wt{


class MotionBlurDemo : public ADemo{
private:
	DECLARE_EVENT_TABLE( MotionBlurDemo );

public:
	bool handleEvent(const Sp<Event> e){
		LOG("CALLBACK!");
		return false;
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

		INIT_EVENT_TABLE( getEventManager() );
		
		
		REGISTER_EVENT_CALLBACK(MousePressEvent::TYPE, &MotionBlurDemo::handleEvent);
	}

	String getConfigFile() const{
		return "";
	}

	String getLevelFile() const{
		return "world.lua";
	}

}; // </MotionBlurDemo>

WT_DECLARE_DEMO(MotionBlurDemo)

}; // </wt>

#endif // </WT_MOTIONBLURDEMO_H>
