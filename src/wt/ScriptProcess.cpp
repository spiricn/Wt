#include "wt/stdafx.h"

#include "wt/ScriptProcess.h"

#define TD_TRACE_TAG "ScriptProcess"

namespace wt{

ScriptProcess::ScriptProcess(lua::ScriptPtr script) : mScript(script),
	mStartFnc( script->getState().Get("onStart") ), mEndFnc( script->getState().Get("onEnd") ),
	mUpdateFnc( script->getState().Get("onUpdate") ){
}

void ScriptProcess::onProcUpdate(float dt){
	mUpdateFnc(dt);
	/*String err = mScript->getParent()->getErrorString();
	if(err.size()){
		TRACEE("Script error occured on update \"%s\"", err.c_str());
	}*/
}

void ScriptProcess::stopProcess(){
	if(isAlive()){
		killProcess();
	}
	else{
		TRACEE("Error stopping process - already stopped");
	}
}

void ScriptProcess::generateMetaTable(){
	expose("stop", &ScriptProcess::stopProcess);
}

void ScriptProcess::onProcStart(){
	LuaObject obj = mScript->getParent()->box(*this);
	
	mScript->getState().Set("Process", obj);
	mStartFnc();
}

void ScriptProcess::onProcEnd(){
	mEndFnc();
}

}; // </wt>