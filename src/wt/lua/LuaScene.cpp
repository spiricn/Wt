#include "wt/stdafx.h"

#include "wt/lua/LuaScene.h"
#include "wt/SceneLoader.h"
#include "wt/ParticleEffectResource.h"
#include "wt/ParticleEffect.h"
#include "wt/CameraController.h"

#define TD_TRACE_TAG "LuaScene"

#define SCENE_OBJ(name, ptr) LUA_OBJ_DEC(Scene, name, ptr)

#define SCENE_ACTOR_OBJ(name, ptr) LUA_OBJ_DEC(ASceneActor, name, ptr)

#define CAM_CTRL_OBJ(name, ptr) LUA_OBJ_DEC(CameraController, name, ptr)

#define TF_OBJ(name, ptr) LUA_OBJ_DEC(ATransformable, name, ptr)

namespace wt
{

namespace lua
{

void Scene_expose(LuaObject obj){
	MODULE_EXPOSE(obj, Scene_findModelledActor);
	MODULE_EXPOSE(obj, Scene_findParticleEffect);
	MODULE_EXPOSE(obj, Scene_findPointLight);
	MODULE_EXPOSE(obj, Scene_findSound);
	MODULE_EXPOSE(obj, Scene_deleteActor);
	MODULE_EXPOSE(obj, Scene_clear);
	MODULE_EXPOSE(obj, Scene_findActor);
	MODULE_EXPOSE(obj, Scene_getCamera);
	MODULE_EXPOSE(obj, Scene_load);
	MODULE_EXPOSE(obj, Scene_createParticleEffect);
	MODULE_EXPOSE(obj, Scene_createCamCtrl);
	MODULE_EXPOSE(obj, Scene_deleteCamCtrl);
	MODULE_EXPOSE(obj, CamCtrl_update);
	MODULE_EXPOSE(obj, CamCtrl_setMode);
	MODULE_EXPOSE(obj, CamCtrl_setTarget);
	MODULE_EXPOSE(obj, Scene_getActorName);
}

const char* Scene_getActorName(void* actorPtr){
	SCENE_ACTOR_OBJ(actor, actorPtr);

	return actor->getName().c_str();
}

void CamCtrl_setMode(void* ctrlPtr, const char* mode){
	CAM_CTRL_OBJ(ctrl, ctrlPtr);

	if(!strcmp(mode, "tps")){
		ctrl->setMode(CameraController::eCTRL_MODE_TPS);
	}
	else if(!strcmp(mode, "fps")){
		ctrl->setMode(CameraController::eCTRL_MODE_FPS);
	}
}

void CamCtrl_setTarget(void* ctrlPtr, void* targetPtr, LuaObject luaOffset){
	TF_OBJ(target, targetPtr);
	CAM_CTRL_OBJ(ctrl, ctrlPtr);	

	glm::vec3 offset;
	luaConv(luaOffset, offset);

	ctrl->setTarget(target, offset);
}

void CamCtrl_update(void* ctrlPtr, float dt){
	CAM_CTRL_OBJ(ctrl, ctrlPtr);

	ctrl->handle(dt);
}

void* Scene_createCamCtrl(void* scenePtr, void* inputPtr){
	SCENE_OBJ(scene, scenePtr);

	AGameInput* input = static_cast<AGameInput*>(inputPtr);

	CameraController* ctrl = new CameraController(input);
	
	ctrl->setCamera(&scene->getCamera());

	return static_cast<void*>(ctrl);
}

void Scene_deleteCamCtrl(void* ctrlPtr){
	CAM_CTRL_OBJ(ctrl, ctrlPtr);

	delete ctrl;
}

void* Scene_createParticleEffect(void* ptr, const char* name, const char* rsrcPath){
	SCENE_OBJ(thiz, ptr);

	ParticleEffect* effect = thiz->createParticleEffect(name);

	ParticleEffectResource* rsrc = thiz->getAssets()->getParticleResourceManager()->getFromPath(rsrcPath);
	if(rsrc == NULL){
		TRACEE("Invalid particle effect resource \"%s\"", rsrc);
		return NULL;
	}

	effect->create(rsrc);

	return dynamic_cast<ASceneActor*>(effect);
}

void Scene_load(void* ptr, const char* path){
	SCENE_OBJ(thiz, ptr);

	SceneLoader loader(thiz, thiz->getAssets());

	loader.load(path);
}

void* Scene_getCamera(void* ptr){
	SCENE_OBJ(thiz, ptr);

	return &thiz->getCamera();
}

void* Scene_findActor(void* ptr, const char* name){
	SCENE_OBJ(thiz, ptr);

	return thiz->findActorByName(name);
}

void Scene_clear(void* ptr){
	SCENE_OBJ(thiz, ptr);

	thiz->clear();
}

void Scene_deleteActor(void* ptr, void* actor){
	SCENE_OBJ(thiz, ptr);

	thiz->deleteActor(static_cast<ASceneActor*>(actor));
}

void* Scene_findParticleEffect(void* ptr, const char* name){
	SCENE_OBJ(thiz, ptr);

	ASceneActor* res = thiz->findActorByName(name);

	return res == NULL ? NULL : res->getActorType() == ASceneActor::eTYPE_PARTICLE_EFFECT ? res : NULL;
}

void* Scene_findPointLight(void* ptr, const char* name){
	SCENE_OBJ(thiz, ptr);

	ASceneActor* res = thiz->findActorByName(name);

	return res == NULL ? NULL : res->getActorType() == ASceneActor::eTYPE_POINT_LIGHT ? res : NULL;
}

void* Scene_findSound(void* ptr, const char* name){
	SCENE_OBJ(thiz, ptr);

	ASceneActor* res = thiz->findActorByName(name);

	return res == NULL ? NULL : res->getActorType() == ASceneActor::eTYPE_SOUND ? res : NULL;
}

void* Scene_findModelledActor(void* ptr, const char* name){
	SCENE_OBJ(thiz, ptr);

	ASceneActor* res = thiz->findActorByName(name);

	return res == NULL ? NULL : res->getActorType() == ASceneActor::eTYPE_MODELLED ? res : NULL;
}



} // </lua>

} // </wt>
