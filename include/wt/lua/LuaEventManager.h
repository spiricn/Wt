#ifndef WT_LUAEVENTMANAGER_H
#define WT_LUAEVENTMANAGER_H

#include "wt/Lua.h"

namespace wt
{

namespace lua
{

void EventManager_expose(LuaObject obj);

void* EventManager_registerListener(void* ptr, const char* evt, LuaObject fnc);

void EventManager_unregisterListener(void* managerPtr, void* listenerPtr, const char* evt);

} // </lua>

} // </wt>

#endif // </WT_LUAEVENTMANAGER_H>