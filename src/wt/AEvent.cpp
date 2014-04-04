#include "wt/stdafx.h"
#include "wt/AEvent.h"

#define TD_TRACE_TAG "AEvent"

namespace wt
{

void AEvent::serialize(LuaObject& dst){
}

void AEvent::deserialize(LuaObject& src){
}

void AEvent::buildLuaData(lua::State* luaState){
	if(!mLuaDataBuilt){
		mLuaData = luaState->newTable();

		mLuaData.Set("eventType", getType().getString().c_str());

		serialize(mLuaData);
		mLuaDataBuilt=true;
	}
}

AEvent::AEvent() : mLuaDataBuilt(false){
}

AEvent::AEvent(LuaObject& data) : mLuaDataBuilt(true), mLuaData(data){
}

AEvent::~AEvent(){
}

const LuaObject& AEvent::getLuaData(){
	WT_ASSERT(mLuaDataBuilt, "Lua data not built");

	return mLuaData;
}

} // </wt>
