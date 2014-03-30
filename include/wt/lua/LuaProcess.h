#ifndef WT_LUAPROCESS_H
#define WT_LUAPROCESS_H

#include "wt/lua/LuaModule.h"
#include "wt/Lua.h"
#include "wt/ProcessManager.h"

namespace wt
{

namespace lua
{

void Process_expose(LuaObject obj);

void* Process_create(void* manager, LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, LuaObject procName);

void* Process_attach(void* manager, void* process);

void Process_kill(void* ptr);

void* Process_setNext(void* proc1, void* proc2);

void Process_suspend(void* procPtr, bool state);

void* Process_spawn(void* managerPtr, LuaObject startFnc, LuaObject updateFnc, LuaObject endFnc, LuaObject luaName);

void* AnimatorProcess_create(void* manager, void* actorPtr);

void* AnimatorProcess_addAnimation(void* procPtr, const char* animation, float speed, bool blend, float blendSpeed, bool loop, float loopDuration);

} // </lua>

} // </wt>

#endif // </WT_LUAPROCESS_H>
