#ifndef WT_AEVENT_H
#define WT_AEVENT_H

#include "wt/stdafx.h"

#include "wt/HashedString.h"
#include "wt/Defines.h"
#include "wt/Sp.h"
#include "wt/lua/State.h"

namespace wt
{

typedef HashedString EventType;

class AEvent{
public:
	AEvent();

	AEvent(LuaObject& data);

	virtual ~AEvent();

	virtual const EventType& getType() const = 0;

	virtual const LuaObject& getLuaData();

protected:
	virtual void serialize(LuaObject& dst);

	virtual void deserialize(LuaObject& src);

	virtual void buildLuaData(lua::State* luaState);

private:
	friend class EventManager;

	LuaObject mLuaData;
	bool mLuaDataBuilt;
}; // </AEvent>

typedef Sp<AEvent> EventPtr;

} // </wt>

#endif // </WT_AEVENT_H>

