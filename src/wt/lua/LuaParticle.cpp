#include "wt/stdafx.h"

#include "wt/lua/LuaParticle.h"
#include "wt/lua/LuaModule.h"

#include "wt/ParticleEffect.h"

#define GET_THIS ParticleEffect* thiz = dynamic_cast<ParticleEffect*>( static_cast<ASceneActor*>( ptr ) )

#define TD_TRACE_TAG "LuaParticle"

namespace wt
{

namespace lua
{

void Particle_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Particle_setVisible);
}

void Particle_setVisible(void* ptr, bool state){
	GET_THIS;

	thiz->setVisible(state);
}

} // </lua>

} // </wt>

