#include "wt/stdafx.h"

#include "wt/lua/LuaActor.h"

namespace wt
{

namespace lua
{

void Actor_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Actor_setPosition);
	MODULE_EXPOSE(obj, Actor_getId);
	MODULE_EXPOSE(obj, Actor_getName);
	MODULE_EXPOSE(obj, Actor_attach);
	MODULE_EXPOSE(obj, Actor_delete);
	MODULE_EXPOSE(obj, Actor_getPosition);
	MODULE_EXPOSE(obj, Actor_getForwardVec);
	MODULE_EXPOSE(obj, Actor_yaw);
	MODULE_EXPOSE(obj, Actor_getRightVec);
	MODULE_EXPOSE(obj, Actor_pitch);
}

void Actor_pitch(void* ptr, float angle){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);
	thiz->getTransformable()->pitch(angle);
}

void Actor_setPosition(void* ptr, LuaObject luaPos){
	ASceneActor* actor = static_cast<ASceneActor*>(ptr);

	glm::vec3 pos;
	luaConv(luaPos, pos);

	actor->getTransformable()->setTranslation(pos);

}

glm::vec3 Actor_getPosition(void* ptr){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	glm::vec3 pos;
	thiz->getTransformable()->getTranslation(pos);

	return pos;
}

glm::vec3 Actor_getForwardVec(void* ptr){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	glm::vec3 fw;
	thiz->getTransformable()->getForwardVector(fw);

	return fw;
}


glm::vec3 Actor_getRightVec(void* ptr){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	glm::vec3 right;
	thiz->getTransformable()->getRightVector(right);

	return right;
}

uint32_t Actor_getId(void* ptr){
	return static_cast<ASceneActor*>(ptr)->getId();
}

const char* Actor_getName(void* ptr){
	return static_cast<ASceneActor*>(ptr)->getName().c_str();
}

bool Actor_attach(void* ptr, void* dst, const char* point){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	ASceneActor* actor = static_cast<ASceneActor*>(dst);

	return thiz->attach(actor, point);
}

void Actor_delete(void* ptr){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	thiz->getScene()->deleteActor(thiz);
}

void Actor_yaw(void* ptr, float angle){
	ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

	thiz->getTransformable()->yaw(angle);
}

} // </lua>

} // </wt>
