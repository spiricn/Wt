#include "wt/stdafx.h"

#include "wt/lua/LuaModelledActor.h"

#define GET_THIS ModelledActor* thiz = dynamic_cast<ModelledActor*>( static_cast<ASceneActor*>(ptr) )

namespace wt
{

namespace lua
{

void ModelledActor_expose(LuaObject obj){
	MODULE_EXPOSE(obj, ModelledActor_playAnimation);
	MODULE_EXPOSE(obj, ModelledActor_setAnimationSpeed);
}

void ModelledActor_playAnimation(void* ptr, const char* name, bool loop){
	GET_THIS;

	thiz->getAnimationPlayer()->play(name, loop);
}

void ModelledActor_setAnimationSpeed(void* ptr, float speed){
	GET_THIS;

	thiz->getAnimationPlayer()->setSpeed(speed);
}

} // </lua>

} // </wt>
