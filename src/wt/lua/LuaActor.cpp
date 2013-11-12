#include "wt/stdafx.h"

#include "wt/lua/LuaActor.h"

#define GET_THIS ASceneActor* thiz = static_cast<ASceneActor*>(ptr);

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
	MODULE_EXPOSE(obj, Actor_getAttachPointPosition);
	MODULE_EXPOSE(obj, Actor_detach);
}

void Actor_detach(void* ptr){
	GET_THIS;

	thiz->detach();
}

glm::vec3 Actor_getAttachPointPosition(void* ptr, const char* id){
	GET_THIS;

	glm::mat4 res;
	if(!thiz->getAttachPointTransform(id, res)){
		TRACEE("Error getting attach point position - invalid point ID \"%s\" for actor \"%s\"", id, thiz->getName().c_str());
		return glm::vec3(1.0f);
	}
	else{
		glm::vec3 pos, scale;
		glm::quat rot;
		math::decomposeMatrix(res, rot, pos, scale);

		return pos;
	}
}

void Actor_pitch(void* ptr, float angle){
	GET_THIS;

	thiz->getTransformable()->pitch(angle);
}

void Actor_setPosition(void* ptr, LuaObject luaPos){
	GET_THIS;

	glm::vec3 pos;
	luaConv(luaPos, pos);

	thiz->getTransformable()->setTranslation(pos);
}

glm::vec3 Actor_getPosition(void* ptr){
	GET_THIS;

	glm::vec3 pos;
	thiz->getTransformable()->getTranslation(pos);

	return pos;
}

glm::vec3 Actor_getForwardVec(void* ptr){
	GET_THIS;

	glm::vec3 fw;
	thiz->getTransformable()->getForwardVector(fw);

	return fw;
}


glm::vec3 Actor_getRightVec(void* ptr){
	GET_THIS;

	glm::vec3 right;
	thiz->getTransformable()->getRightVector(right);

	return right;
}

uint32_t Actor_getId(void* ptr){
	GET_THIS;

	return thiz->getId();
}

const char* Actor_getName(void* ptr){
	GET_THIS;

	return thiz->getName().c_str();
}

bool Actor_attach(void* ptr, void* dst, const char* point){
	GET_THIS;

	ASceneActor* actor = static_cast<ASceneActor*>(dst);

	return thiz->attach(actor, point);
}

void Actor_delete(void* ptr){
	GET_THIS;

	thiz->getScene()->deleteActor(thiz);
}

void Actor_yaw(void* ptr, float angle){
	GET_THIS;

	thiz->getTransformable()->yaw(angle);
}

} // </lua>

} // </wt>
