#include "wt/stdafx.h"

#include "wt/ScriptProcess.h"

#define TD_TRACE_TAG "ScriptProcess"


#define PROCESS_START_FNC_NAME "onProcessStart"

#define PROCESS_END_FNC_NAME "onProcessEnd"

#define PROCESS_UPDATE_FNC_NAME "onProcessUpdate"

namespace wt
{

ScriptProcess::ScriptProcess(lua::ScriptPtr script) : mScript(script), mStartFnc(NULL), mEndFnc(NULL), mUpdateFnc(NULL){
	// Start callback
	lua::LuaObject procStartFnc = script->getState().Get(PROCESS_START_FNC_NAME);
	if(procStartFnc.IsFunction()){
		mStartFnc.set( new CallbackFnc( procStartFnc ) );
	}

	// End callback
	lua::LuaObject procEndFnc = script->getState().Get(PROCESS_END_FNC_NAME);
	if(procEndFnc.IsFunction()){
		mEndFnc.set( new CallbackFnc( procEndFnc ) );
	}

	// Update callback
	lua::LuaObject procUpdateFnc = script->getState().Get(PROCESS_UPDATE_FNC_NAME);
	if(procUpdateFnc.IsFunction()){
		mUpdateFnc.set( new CallbackFnc( procUpdateFnc ) );
	}
}

ScriptProcess::~ScriptProcess(){
}

void ScriptProcess::onProcUpdate(float dt){
	if(mUpdateFnc != NULL){
		try{
			(*mUpdateFnc)(dt);
		}catch(LuaPlus::LuaException& e){
#if 0
			// TODO this causes an abort() for some reason
			TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
#else
			TRACEE("Script error ocurred \"%s\"", e.GetErrorMessage());
#endif
		}
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

		if(mStartFnc != NULL){
			(*mStartFnc)();
		}
	}catch(...){
		TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
	}
}

void ScriptProcess::onProcEnd(){
	if(mEndFnc != NULL){
		try{
			(*mEndFnc)();
		}catch(...){
			TRACEE("Script error ocurred %s", mScript->getParent()->getErrorString().c_str());
		}
	}
}

}; // </wt>