#ifndef WT_SCRIPTEVENT_H
#define WT_SCRIPTEVENT_H

#include "wt/Event.h"

namespace wt
{

class ScriptEvent : public Event{
public:
	ScriptEvent(EvtType type, LuaObject& data);

	const EvtType& getType() const;

private:
	EvtType mEventType;
	LuaObject mData;

	void serialize(LuaObject& dst);

	void deserialize(LuaObject& src);

}; // </ScriptEvent>

} // </wt>

#endif // </WT_SCRIPTEVENT_H>
