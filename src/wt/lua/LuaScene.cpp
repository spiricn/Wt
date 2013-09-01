#include "wt/stdafx.h"

#include "wt/lua/LuaScene.h"
#include "wt/SceneLoader.h"

#define TD_TRACE_TAG "LuaScene"

#define GET_THIS Scene* thiz = static_cast<Scene*>(ptr);

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
}

void* Scene_createParticleEffect(void* ptr, const char* name, const char* rsrcPath){
	GET_THIS;

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
	GET_THIS;

	SceneLoader loader(thiz, thiz->getAssets());

	loader.load(path);
}

void* Scene_getCamera(void* ptr){
	GET_THIS;

	return &thiz->getCamera();
}

void* Scene_findActor(void* ptr, const char* name){
	GET_THIS;

	return thiz->findActorByName(name);
}

void Scene_clear(void* ptr){
	GET_THIS;

	thiz->clear();
}

void Scene_deleteActor(void* ptr, void* actor){
	GET_THIS;

	thiz->deleteActor(static_cast<ASceneActor*>(actor));
}

void* Scene_findParticleEffect(void* ptr, const char* name){
	GET_THIS;

	ASceneActor* res = thiz->findActorByName(name);

	return res->getActorType() == ASceneActor::eTYPE_PARTICLE_EFFECT ? res : NULL;
}

void* Scene_findPointLight(void* ptr, const char* name){
	GET_THIS;

	ASceneActor* res = thiz->findActorByName(name);

	return res->getActorType() == ASceneActor::eTYPE_POINT_LIGHT ? res : NULL;
}

void* Scene_findSound(void* ptr, const char* name){
	GET_THIS;

	ASceneActor* res = thiz->findActorByName(name);

	return res->getActorType() == ASceneActor::eTYPE_SOUND ? res : NULL;
}

void* Scene_findModelledActor(void* ptr, const char* name){
	GET_THIS;

	ASceneActor* res = thiz->findActorByName(name);

	return res->getActorType() == ASceneActor::eTYPE_MODELLED ? res : NULL;
}



} // </lua>

} // </wt>
