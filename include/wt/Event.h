#ifndef WT_EVENT_H
#define WT_EVENT_H


#include "wt/stdafx.h"

#include "wt/HashedString.h"
#include "wt/Defines.h"
#include "wt/Sp.h"
#include "wt/lua/State.h"

namespace wt{

typedef HashedString EvtType;

class EventManager;

class Event{
friend class EventManager;

private:
	LuaObject mLuaData;
	bool mLuaDataBuilt;
	uint32_t mEmitterData;

protected:

	virtual void serialize(LuaObject& dst) = 0;

	virtual void deserialize(LuaObject& src) = 0;

	virtual void buildLuaData(lua::State* luaState){
		if(!mLuaDataBuilt){
			mLuaData = luaState->newTable();

			mLuaData.Set("eventType", getType().getString().c_str());

			serialize(mLuaData);
			mLuaDataBuilt=true;
		}
	}

public:
	Event() : mLuaDataBuilt(false){
	}

	Event(LuaObject& data) : mLuaDataBuilt(true), mLuaData(data){
	}

	uint32_t getEmitterData() const{
		return mEmitterData;
	}

	void setEmitterData(uint32_t data){
		mEmitterData = data;
	}

	virtual ~Event(){
	}

	virtual const EvtType& getType() const = 0;

	virtual const LuaObject& getLuaData(){
		WT_ASSERT(mLuaDataBuilt, "Lua data not built");

		return mLuaData;
	}

}; // </Event>

typedef Sp<Event> EvtPtr;

class ScriptEvent : public Event{
private:
	EvtType mEventType;
	LuaObject mData;

	void serialize(LuaObject& dst){
		dst = mData;
	}

	void deserialize(LuaObject& src){
		mData = src;
	}

public:
	ScriptEvent(EvtType type, LuaObject& data) : mEventType(type), Event(data){
	}

	const EvtType& getType() const {
		return mEventType;
	}

}; // </ScriptEvent>

}; // </wt>

#endif
