#ifndef WT_LUAENGINE_H
#define WT_LUAENGINE_H

#include "wt/lua/LuaModule.h"
#include "wt/ASceneActor.h"
#include "wt/Lua.h"
#include "wt/ModelledActor.h"
#include "wt/Scene.h"

namespace wt
{

namespace lua
{

void Engine_expose(LuaObject obj);

void* Engine_getInput(void* enginePtr);

} // </lua>

} // </wt>


#endif // </WT_LUAENGINE_H>
