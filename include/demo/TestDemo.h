#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"

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
	}

	String getConfigFile() const{
		return "";
	}

	String getLevelFile() const{
		return "";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
