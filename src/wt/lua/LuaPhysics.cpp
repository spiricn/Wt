#include "wt/stdafx.h"

#include "wt/lua/LuaPhysics.h"
#include "wt/lua/LuaModule.h"

#include "wt/Physics.h"

#define TD_TRACE_TAG "LuaPhysics"


#define PX_ACTOR_OBJ(name, ptr) LUA_OBJ_DEC(PhysicsActor, name, ptr)

namespace wt
{

namespace lua
{

void Physics_expose(LuaPlus::LuaObject state){
	MODULE_EXPOSE(state, Physics_getSceneActor);
	MODULE_EXPOSE(state, Physics_getActorName);
}

void* Physics_getSceneActor(void* physicsActorPtr){
	PX_ACTOR_OBJ(actor, physicsActorPtr);

	return static_cast<void*>(actor->getSceneActor());
}

const char* Physics_getActorName(void* physicsActorPtr){
	PX_ACTOR_OBJ(actor, physicsActorPtr);

	return actor->getName().c_str();
}

} // </lua>

} // </wt>

