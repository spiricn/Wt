#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/gui/ListView.h>

#include <wt/MusicPlayer.h>
#include <wt/OwnPtr.h>
#include <wt/ProcessSpawner.h>
using namespace wt;

#define TD_TRACE_TAG "TestDemo"

class TestDemo : public ADemo{
public:
	TestDemo(){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt);
	}
	
	void onKeyUp(wt::VirtualKey code){
		ADemo::onKeyUp(code);
	}
	
	void onDemoStart(const LuaObject& config){
	}

	String getScriptPath() const{
		return "demo_workspace/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO_IMPL(TestDemo);
