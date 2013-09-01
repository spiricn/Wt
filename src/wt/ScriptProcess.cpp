#include "wt/stdafx.h"

#include "wt/ScriptProcess.h"

#define TD_TRACE_TAG "ScriptProcess"

namespace wt{

ScriptProcess::ScriptProcess(lua::ScriptPtr script) : mScript(script),
	mStartFnc(script->getState().Get("onProcessStart")), mEndFnc(script->getState().Get("onProcessEnd")), mUpdateFnc(script->getState().Get("onProcessUpdate")){
}

ScriptProcess::~ScriptProcess(){
}

void ScriptProcess::onProcUpdate(float dt){
	try{
		mUpdateFnc(dt);
	}catch(...){
		TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
	}
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
	try{
		LuaObject obj = mScript->getParent()->box(*this);
	
		mScript->getState().Set("Process", obj);

		mStartFnc();
	}catch(...){
		TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
	}
}

void ScriptProcess::onProcEnd(){
	try{
		mEndFnc();
	}catch(...){
		TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
	}
}

}; // </wt>