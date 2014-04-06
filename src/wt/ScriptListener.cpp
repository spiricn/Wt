/**
 * @file ScriptListener.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/ScriptListener.h"
#include "wt/EventManager.h"

#define TD_TRACE_TAG "ScriptListener"

namespace wt
{

ScriptEventListener::ScriptEventListener(LuaObject& callback) : mCallback(callback){
	WT_ASSERT(callback.IsFunction(), "Invalid lua callback function; expected type \"function\", got \"%s\"", callback.TypeName());
}

bool ScriptEventListener::handleEvent(const EventPtr event){
	const LuaObject& evtData = const_cast<AEvent*>(event.get())->getLuaData();

	LuaFunction<bool> func(mCallback);

	bool res=true;

	//event->buildLuaData(
	try{
		res = func(evtData);
	}catch(LuaException e){
		TRACEE("Script callback function error: \"%s\"", 
			e.GetErrorMessage());
	}

	return res;
}
	
} // </wt>

