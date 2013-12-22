#include "wt/stdafx.h"
#include "wt/Event.h"

#define TD_TRACE_TAG "Event"

namespace wt
{

void Event::serialize(LuaObject& dst){
}

void Event::deserialize(LuaObject& src){
}

void Event::buildLuaData(lua::State* luaState){
	if(!mLuaDataBuilt){
		mLuaData = luaState->newTable();

		mLuaData.Set("eventType", getType().getString().c_str());

		serialize(mLuaData);
		mLuaDataBuilt=true;
	}
}

Event::Event() : mLuaDataBuilt(false){
}

Event::Event(LuaObject& data) : mLuaDataBuilt(true), mLuaData(data){
}

uint32_t Event::getEmitterData() const{
	return mEmitterData;
}

void Event::setEmitterData(uint32_t data){
	mEmitterData = data;
}

Event::~Event(){
}


const LuaObject& Event::getLuaData(){
	WT_ASSERT(mLuaDataBuilt, "Lua data not built");

	return mLuaData;
}

} // </wt>
