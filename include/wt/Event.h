#ifndef WT_EVENT_H
#define WT_EVENT_H

#include "wt/stdafx.h"

#include "wt/HashedString.h"
#include "wt/Defines.h"
#include "wt/Sp.h"
#include "wt/lua/State.h"

namespace wt
{

typedef HashedString EvtType;

class Event{
public:
	Event();

	Event(LuaObject& data);

	uint32_t getEmitterData() const;

	void setEmitterData(uint32_t data);

	virtual ~Event();

	virtual const EvtType& getType() const = 0;

	virtual const LuaObject& getLuaData();

protected:
	virtual void serialize(LuaObject& dst);

	virtual void deserialize(LuaObject& src);

	virtual void buildLuaData(lua::State* luaState);

private:
	friend class EventManager;

	LuaObject mLuaData;
	bool mLuaDataBuilt;
	uint32_t mEmitterData;
}; // </Event>

typedef Sp<Event> EvtPtr;

}; // </wt>

#endif
