#ifndef WT_LUAMODELLEDACTOR_H
#define WT_LUAMODELLEDACTOR_H

#include "wt/lua/LuaModule.h"
#include "wt/ASceneActor.h"
#include "wt/Lua.h"
#include "wt/ModelledActor.h"
#include "wt/Scene.h"

namespace wt
{

namespace lua
{

void ModelledActor_expose(LuaObject obj);

bool ModelledActor_isAnimationPlaying(void* ptr);

void ModelledActor_playAnimation(void* ptr, const char* name, bool loop);

void ModelledActor_setAnimationSpeed(void* ptr, float speed);

float ModelledActor_getAnimationPos(void* ptr);

} // </lua>

} // </wt>


#endif // </WT_LUAACTOR_H>
