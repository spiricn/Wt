#ifndef WT_AEVENT_H
#define WT_AEVENT_H

#include "wt/stdafx.h"

#include "wt/HashedString.h"
#include "wt/Defines.h"
#include "wt/Sp.h"
#include "wt/lua/State.h"

namespace wt
{

class IEventEmitter;

class EventManager;

typedef HashedString EventType;

class AEvent{
public:
	AEvent();

	AEvent(LuaObject& data);

	virtual ~AEvent();

	virtual const EventType& getType() const = 0;

	virtual const LuaObject& getLuaData();

	EventManager* getManager() const;

	IEventEmitter* getEmitter() const;

protected:
	virtual void serialize(LuaObject& dst);

	virtual void deserialize(LuaObject& src);

	virtual void buildLuaData(lua::State* luaState);

	void setEmitter(IEventEmitter* emitter);

private:
	void setManager(EventManager* manager);

private:
	LuaObject mLuaData;
	bool mLuaDataBuilt;
	EventManager* mManager;
	IEventEmitter* mEmitter;

private:
	friend class EventManager;
}; // </AEvent>

typedef Sp<AEvent> EventPtr;

} // </wt>

#endif // </WT_AEVENT_H>

