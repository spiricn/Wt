#ifndef WT_LUASCENE_H
#define WT_LUASCENE_H

#include "wt/lua/LuaModule.h"
#include "wt/ASceneActor.h"
#include "wt/Lua.h"
#include "wt/ModelledActor.h"
#include "wt/Scene.h"

namespace wt
{

namespace lua
{

void Scene_expose(LuaObject obj);

void Scene_load(void* ptr, const char* path);

void* Scene_getCamera(void* ptr);

void Scene_clear(void* ptr);

void* Scene_findActor(void* ptr, const char* name);

void* Scene_findModelledActor(void* ptr, const char* name);

void* Scene_findParticleEffect(void* ptr, const char* name);

void* Scene_findPointLight(void* ptr, const char* name);

void* Scene_findSound(void* ptr, const char* name);

void* Scene_findModelledActor(void* ptr, const char* name);

void Scene_deleteActor(void* ptr, void* actor);

void* Scene_createParticleEffect(void* ptr, const char* name, const char* rsrc);

} // </lua>

} // </wt>


#endif // </WT_LUASCENE_H>
