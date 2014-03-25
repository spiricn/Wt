#ifndef WT_LUABINDINGS_H
#define WT_LUABINDINGS_H

#include "wt/lua/LuaModelledActor.h"
#include "wt/lua/LuaActor.h"
#include "wt/lua/LuaScene.h"
#include "wt/lua/LuaCamera.h"
#include "wt/lua/LuaPointLight.h"
#include "wt/lua/LuaSound.h"
#include "wt/lua/LuaEventManager.h"
#include "wt/lua/LuaParticle.h"
#include "wt/lua/LuaProcess.h"

namespace wt
{

namespace lua
{

inline void LuaBindings_expose(LuaObject obj){
	Scene_expose(obj);
	ModelledActor_expose(obj);
	Actor_expose(obj);
	Camera_expose(obj);
	PointLight_expose(obj);
	Sound_expose(obj);
	EventManager_expose(obj);
	Particle_expose(obj);
	Process_expose(obj);
}

} // </lua>

} // </wt>

#endif // </WT_LUABINDINGS_H.
