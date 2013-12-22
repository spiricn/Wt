#include "wt/stdafx.h"
#include "wt/ScriptEvent.h"

#define TD_TRACE_TAG "ScriptEvent"

namespace wt
{

ScriptEvent::ScriptEvent(EvtType type, LuaObject& data) : mEventType(type), Event(data){
}

const EvtType& ScriptEvent::getType() const {
	return mEventType;
}

void ScriptEvent::serialize(LuaObject& dst){
	dst = mData;
}

void ScriptEvent::deserialize(LuaObject& src){
	mData = src;
}
	
} // </wt>
