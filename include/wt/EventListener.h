#ifndef WT_EVENTLISTENER_H
#define WT_EVENTLISTENER_H


#include "wt/stdafx.h"

#include "wt/AEvent.h"
#include "wt/Sp.h"

namespace wt{



class ScriptEventListener : public IEventListener{
private:
	LuaObject mCallback;
public:
	ScriptEventListener(LuaObject& callback) : mCallback(callback){
		WT_ASSERT(callback.IsFunction(), "Invalid lua callback function");
	}

	bool handleEvent(const Sp<AEvent> evt){
		const LuaObject& evtData = const_cast<AEvent*>(evt.get())->getLuaData();

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
