#ifndef WT_LUAACTOR_H
#define WT_LUAACTOR_H

#include "wt/ASceneActor.h"
#include "wt/Lua.h"
#include "wt/lua/LuaModule.h"
#include "wt/ModelledActor.h"
#include "wt/Scene.h"

namespace wt
{

namespace lua
{

void Actor_expose(LuaObject obj);

void Actor_detach(void* ptr);

void Actor_pitch(void* ptr, float angle);

void Actor_setPosition(void* ptr, LuaObject luaPos);

glm::vec3 Actor_getPosition(void* ptr);

glm::vec3 Actor_getForwardVec(void* ptr);

glm::vec3 Actor_getRightVec(void* ptr);

uint32_t Actor_getId(void* ptr);

const char* Actor_getName(void* ptr);

void ModelledActor_playAnimation(void* ptr, const char* name, bool loop);

bool Actor_attach(void* ptr, void* dst, const char* point);

void Actor_delete(void* ptr);

void Actor_yaw(void* ptr, float angle);

glm::vec3 Actor_getAttachPointPosition(void* ptr, const char* id);

} // </lua>

} // </wt>


#endif // </WT_LUAACTOR_H>
