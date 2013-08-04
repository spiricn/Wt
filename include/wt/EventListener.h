#ifndef WT_EVENTLISTENER_H
#define WT_EVENTLISTENER_H


#include "wt/stdafx.h"

#include "wt/Event.h"
#include "wt/Sp.h"

namespace wt{

#define TD_TRACE_TAG "EventListener"

class EventListener{
public:
	virtual bool handleEvent(const Sp<Event> e) = 0 {
		return true;
	}

	virtual ~EventListener(){
	}

}; // </EventListener>


class ScriptEventListener : public EventListener{
private:
	LuaObject mCallback;
public:
	ScriptEventListener(LuaObject& callback) : mCallback(callback){
		WT_ASSERT(callback.IsFunction(), "Invalid lua callback function");
	}

	bool handleEvent(const Sp<Event> evt){
		const LuaObject& evtData = const_cast<Event*>(evt.get())->getLuaData();

		LuaFunction<bool> func(mCallback);
		bool res=true;

		try{
			res = func(evtData);
		}catch(LuaException e){
			TRACEE("Script callback function error: \"%s\"", 
				e.GetErrorMessage());
		}

		return res;
	}

}; // </ScriptEventListener>


}; // </wt>

#endif
