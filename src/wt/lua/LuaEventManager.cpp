#include "wt/stdafx.h"

#include "wt/lua/LuaEventManager.h"
#include "wt/lua/LuaModule.h"
#include "wt/EventManager.h"
#include "wt/ScriptListener.h"

#define EVT_MANAGER_OBJ(name, ptr) LUA_OBJ_DEC(EventManager, name, ptr)

#define SCRIPT_LISTENER_OBJ(name, ptr) LUA_OBJ_DEC(ScriptEventListener, name, ptr)

namespace wt
{

namespace lua
{

void EventManager_expose(LuaObject state){
	MODULE_EXPOSE(state, EventManager_registerListener);
	MODULE_EXPOSE(state, EventManager_unregisterListener);
}

void* EventManager_registerListener(void* ptr, const char* evt, LuaObject fnc){
	EVT_MANAGER_OBJ(manager, ptr);

	ScriptEventListener* listener = new ScriptEventListener(fnc);

	manager->registerListener(listener, EventType(evt), EventManager::eCONNECTION_QUEUED, NULL, true);

	return static_cast<void*>(listener);
}

void EventManager_unregisterListener(void* managerPtr, void* listenerPtr, const char* evt){
	EVT_MANAGER_OBJ(manager, managerPtr);
	SCRIPT_LISTENER_OBJ(listener, listenerPtr);

	manager->unregisterListener(listener, EventType(evt));
}

} // </lua>

} // </wt>
