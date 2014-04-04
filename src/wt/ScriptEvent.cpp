#include "wt/stdafx.h"
#include "wt/ScriptEvent.h"

#define TD_TRACE_TAG "ScriptEvent"

namespace wt
{

ScriptEvent::ScriptEvent(EventType type, LuaObject& data) : mEventType(type), AEvent(data){
}

const EventType& ScriptEvent::getType() const {
	return mEventType;
}

void ScriptEvent::serialize(LuaObject& dst){
	dst = mData;
}

void ScriptEvent::deserialize(LuaObject& src){
	mData = src;
}
	
} // </wt>
