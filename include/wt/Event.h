#ifndef WT_EVENT_H
#define WT_EVENT_H


#include "wt/stdafx.h"

#include "wt/HashedString.h"
#include "wt/LuaStateManager.h"
#include "wt/Defines.h"
#include "wt/Sp.h"

namespace wt{

typedef HashedString EvtType;

class Event{
private:
	LuaObject mLuaData;
	bool mLuaDataBuilt;
	Uint32 mEmitterData;

protected:

	virtual void serialize(LuaObject& dst) = 0;

	virtual void deserialize(LuaObject& src) = 0;

public:
	Event() : mLuaDataBuilt(false){
	}

	Event(LuaObject& data) : mLuaDataBuilt(true), mLuaData(data){
	}

	Uint32 getEmitterData() const{
		return mEmitterData;
	}

	void setEmitterData(Uint32 data){
		mEmitterData = data;
	}

	virtual ~Event(){
	}

	virtual const EvtType& getType() const = 0;

	virtual const LuaObject& getLuaData(){
		if(!mLuaDataBuilt){
			LUA_NEW_TABLE( mLuaData );
			mLuaData.Set("eventType", getType().getString().c_str());

			serialize(mLuaData);
			mLuaDataBuilt=true;
		}

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
