#include "demo/stdafx.h"

#include "demo/ADemo.h"

using namespace wt;

#define TD_TRACE_TAG "BasicDemo"

class ScriptDemo : public ADemo{
public:
	ScriptDemo(){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt, getInput());
	}
	void onDemoStart(const LuaObject&){
	}

	void onDemoStop(){
	}

	String getScriptPath() const{
		return "demo_workspace/ScriptDemo/main.lua";
	}
private:

}; // </ScriptDemo>

WT_DECLARE_DEMO_IMPL(ScriptDemo);
