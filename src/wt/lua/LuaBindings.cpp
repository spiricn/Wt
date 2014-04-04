#include "wt/stdafx.h"

#include "wt/lua/LuaBindings.h"
#include "wt/lua/LuaModelledActor.h"
#include "wt/lua/LuaActor.h"
#include "wt/lua/LuaScene.h"
#include "wt/lua/LuaCamera.h"
#include "wt/lua/LuaPointLight.h"
#include "wt/lua/LuaSound.h"
#include "wt/lua/LuaEventManager.h"
#include "wt/lua/LuaParticle.h"
#include "wt/lua/LuaProcess.h"
#include "wt/lua/LuaActorMoveControler.h"
#include "wt/lua/LuaEngine.h"
#include "wt/lua/LuaPhysics.h"


namespace wt
{

namespace lua
{

void LuaBindings_expose(LuaPlus::LuaObject obj){
	Scene_expose(obj);
	ModelledActor_expose(obj);
	Actor_expose(obj);
	Camera_expose(obj);
	PointLight_expose(obj);
	Sound_expose(obj);
	EventManager_expose(obj);
	Particle_expose(obj);
	Process_expose(obj);
	MoveCtrl_expose(obj);
	Engine_expose(obj);
	Physics_expose(obj);
}

} // </lua>

} // </wt>

