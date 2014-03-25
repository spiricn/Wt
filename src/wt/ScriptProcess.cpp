#include "wt/stdafx.h"

#include "wt/ScriptProcess.h"

#define TD_TRACE_TAG "ScriptProcess"


namespace wt
{

ScriptProcess::ScriptProcess(LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, const String& name) : mStartFnc(NULL), mEndFnc(NULL), mUpdateFnc(NULL), AProcess(name){
	// Start callback
	if(startFnc.IsFunction()){
		mStartFnc.set( new CallbackFnc( startFnc ) );
	}
	else if(!startFnc.IsNil()){
		TRACEW("Invalid start function (type=%s; expected=function)", startFnc.TypeName());
	}

	// End callback
	if(updateFnc.IsFunction()){
		mUpdateFnc.set( new CallbackFnc( updateFnc ) );
	}
	else if(!updateFnc.IsNil()){
		TRACEW("Invalid update function (type=%s; expected=function)", updateFnc.TypeName());
	}

	// Update callback
	if(endFnc.IsFunction()){
		mEndFnc.set( new CallbackFnc( endFnc ) );
	}
	else if(!endFnc.IsNil()){
		TRACEW("Invalid end function (type=%s; expected=function)", endFnc.TypeName());
	}
}

ScriptProcess::~ScriptProcess(){
}

void ScriptProcess::onProcUpdate(float dt){
	if(mUpdateFnc != NULL){
		try{
			(*mUpdateFnc)(dt);
		}catch(LuaPlus::LuaException& e){
			TRACEE("Script error ocurred \"%s\"", e.GetErrorMessage());
		}
	}
}

void ScriptProcess::onProcStart(){
	if(mStartFnc != NULL){
		try{
			(*mStartFnc)();
		}catch(LuaPlus::LuaException& e){
			TRACEE("Script error ocurred \"%s\"", e.GetErrorMessage());
		}
	}
}

void ScriptProcess::onProcEnd(){
	if(mEndFnc != NULL){
		try{
			(*mEndFnc)();
		}catch(LuaPlus::LuaException& e){
			TRACEE("Script error ocurred \"%s\"", e.GetErrorMessage());
		}
	}
}

}; // </wt>