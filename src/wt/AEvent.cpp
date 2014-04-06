#include "wt/stdafx.h"
#include "wt/AEvent.h"

#include "wt/EventManager.h"

#define TD_TRACE_TAG "AEvent"

namespace wt
{

AEvent::AEvent() : mLuaDataBuilt(false), mManager(NULL){
}

AEvent::AEvent(LuaObject& data) : mLuaDataBuilt(true), mLuaData(data){
}

AEvent::~AEvent(){
}

void AEvent::serialize(LuaObject& dst){
	dst.Set("eventType", getType().c_str());
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

const LuaObject& AEvent::getLuaData(){
	if(!mLuaDataBuilt){
		buildLuaData( mManager->getLuaState() );
	}

	return mLuaData;
}

EventManager* AEvent::getManager() const{
	return mManager;
}

void AEvent::setManager(EventManager* manager){
	mManager = manager;
}

IEventEmitter* AEvent::getEmitter() const{
	return mEmitter;
}

void AEvent::setEmitter(IEventEmitter* emitter){
	mEmitter = emitter;
}

} // </wt>
