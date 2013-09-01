#include "wt/stdafx.h"

#include "wt/lua/LuaEventManager.h"
#include "wt/lua/LuaModule.h"
#include "wt/EventManager.h"

#define GET_THIS EventManager* thiz = static_cast<EventManager*>( ptr )

#define TD_TRACE_TAG "LuaEventManager"

namespace wt
{

namespace lua
{

void EventManager_expose(LuaObject obj){
	MODULE_EXPOSE(obj, EventManager_registerListener);
}

void EventManager_registerListener(void* ptr, const char* evt, LuaObject fnc){
	GET_THIS;

	thiz->addScriptListener(evt, fnc);
}

} // </lua>

} // </wt>

