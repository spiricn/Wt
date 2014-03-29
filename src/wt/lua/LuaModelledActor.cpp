#include "wt/stdafx.h"

#include "wt/lua/LuaModelledActor.h"

#define ACTOR_OBJ(name, ptr) \
	WT_ASSERT( (ptr) != NULL, "Invalid actor object pointer" ); \
	ModelledActor* name = dynamic_cast<ModelledActor*>( static_cast<ASceneActor*>(ptr) )

namespace wt
{

namespace lua
{

void ModelledActor_expose(LuaObject obj){
	MODULE_EXPOSE(obj, ModelledActor_playAnimation);
	MODULE_EXPOSE(obj, ModelledActor_setAnimationSpeed);
	MODULE_EXPOSE(obj, ModelledActor_isAnimationPlaying);
	MODULE_EXPOSE(obj, ModelledActor_getAnimationPos);
}

float ModelledActor_getAnimationPos(void* ptr){
	ACTOR_OBJ(thiz, ptr);

	return thiz->getAnimationPlayer()->getPosition();
}

bool ModelledActor_isAnimationPlaying(void* ptr){
	ACTOR_OBJ(thiz, ptr);

	return thiz->getAnimationPlayer()->isPlaying();
}

void ModelledActor_playAnimation(void* ptr, const char* name, bool loop){
	ACTOR_OBJ(thiz, ptr);

	thiz->getAnimationPlayer()->play(name, loop);
}

void ModelledActor_setAnimationSpeed(void* ptr, float speed){
	ACTOR_OBJ(thiz, ptr);

	thiz->getAnimationPlayer()->setSpeed(speed);
}

} // </lua>

} // </wt>
