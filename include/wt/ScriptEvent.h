#ifndef WT_SCRIPTEVENT_H
#define WT_SCRIPTEVENT_H

#include "wt/AEvent.h"

namespace wt
{

class ScriptEvent : public AEvent{
public:
	ScriptEvent(EventType type, LuaObject& data);

	const EventType& getType() const;

private:
	EventType mEventType;
	LuaObject mData;

	void serialize(LuaObject& dst);

	void deserialize(LuaObject& src);

}; // </ScriptEvent>

} // </wt>

#endif // </WT_SCRIPTEVENT_H>
