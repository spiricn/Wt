#include "demo/stdafx.h"

#include "demo/ADemo.h"

using namespace wt;

#define TD_TRACE_TAG "TestDemo"

class TestDemo : public ADemo{
public:
	TestDemo(){
		LOG("Test");
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt, getInput());
	}
	
	void onKeyUp(wt::VirtualKey code){
		ADemo::onKeyUp(code);
	}


	String getScriptPath() const{
		return "demo_workspace/TestDemo/main.lua";
	}
};

WT_DECLARE_DEMO_IMPL(TestDemo);
