
#include "wt/stdafx.h"

#include "wt/Scene.h"

#define TD_TRACE_TAG "Scene"

namespace wt{

Scene::Scene(Physics* physics, Assets* assets) : 
	mSkyBox(NULL), mNumPointLights(0), mNumSpotLights(0), mAssets(assets), mPhysics(physics){
		setCamera(&mDefaultCamera);
}

void Scene::addSpotLight(const SpotLight& light){
	mSpotLights[mNumSpotLights++] = light;
}


SpotLight& Scene::getSpotLight(uint32_t index){
	return mSpotLights[index];
}

Scene::~Scene(){
	destroy();
}

Fog& Scene::getFog(){
	return mFog;
}

Scene::ActorMap& Scene::getActorMap(){
	return mActors;
}

Physics* Scene::getPhysics() const{
	return mPhysics;
}

void Scene::setCamera(math::Camera* camera){
	mCamera = camera;
}

void Scene::addPointLight(const PointLight& light){
	mPointLights[mNumPointLights++] = light;
}

PointLight& Scene::getPointLight(uint32_t index){
	return mPointLights[index];
}

DirectionalLight& Scene::getDirectionalLight(){
	return mDirectionalLight;
}

uint32_t Scene::getNumPointLights() const{
	return mNumPointLights;
}

#if 0
void Scene::load(const String& path){
	// scene

}
#endif


uint32_t Scene::generateActorId(){
	uint32_t id=mActors.size();
	while(true){
		if(getActorById(id) != NULL){
			++id;
		}
		else{
			break;
		}
	}
	return id;
}

void Scene::deleteActor(ASceneActor* aActor){
	eraseActor( mActors.find(aActor->getId()) );
}

Scene::ActorMap::iterator Scene::eraseActor(ActorMap::iterator& iter){
	WT_ASSERT(iter != mActors.end(), "Invalid actor iterator");

	if(iter->second->getPhysicsActor()){
		mPhysics->removeActor( iter->second->getPhysicsActor() );
	}

	if(iter->second->getActorType() == ASceneActor::eTYPE_MODELLED){
		WT_ASSERT(mModelledActors.erase( static_cast<ModelledActor*>(iter->second ) ) == 1, "Error erasing actor");
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_TERRAIN){
		WT_ASSERT(mTerrainSet.erase( static_cast<Terrain*>(iter->second) ) == 1, "Error erasing actor");
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_PARTICLE_EFFECT){
		WT_ASSERT(mParticleEffects.erase( static_cast<ParticleEffect*>(iter->second) ), "Error erasing actor");
	}

	delete iter->second;

	return mActors.erase(iter);
}

void Scene::clear(){
	setSkyBox(NULL);
	
	ActorMap::iterator iter = mActors.begin();

	while(iter != mActors.end()){
		eraseActor(iter++);
	}
	
	mDefaultCamera = math::Camera();
}

ASceneActor* Scene::getActorById(uint32_t id){
	ActorMap::iterator i = mActors.find(id);
	return i == mActors.end() ? NULL : i->second;
}

Terrain* Scene::createTerrain(const String& name){
	Terrain* terrain = new Terrain(this, generateActorId(), name);

	mActors.insert( std::make_pair(terrain->getId(), terrain) );

	mTerrainSet.insert(terrain);

	return terrain;
}

ParticleEffect* Scene::createParticleEffect(const String& name){
	ParticleEffect* effect = new ParticleEffect(this, generateActorId(), name);

	mActors.insert( std::make_pair(effect->getId(), effect) );

	mParticleEffects.insert(effect);

	return effect;
}

ModelledActor* Scene::createModelledActor(const String& name){
	ModelledActor* actor = new ModelledActor(this, generateActorId(), name);

	mActors.insert( std::make_pair(actor->getId(), actor) );

	mModelledActors.insert(actor);

	return actor;
}

ASceneActor* Scene::findActorByName(const String& name) const{
	for(ActorMap::const_iterator i=mActors.cbegin(); i!=mActors.cend(); i++){
		if(i->second->getName().compare(name) == 0){
			return i->second;
		}
	}

	return NULL;
}

void Scene::destroy(){
	clear();
}

void Scene::update(float dt){
	for(ActorMap::iterator i=mActors.begin(); i!=mActors.end(); i++){
		i->second->update(dt);
	}
}

math::Camera& Scene::getCamera(){
	return *mCamera;
}

void Scene::setSkyBox(SkyBox* sky){
	mSkyBox = sky;
}

SkyBox* Scene::getSkyBox(){
	return mSkyBox;
}

/*********************************************************************
 ************** Lua bindings *****************************************
 *********************************************************************/

#define LUA_ERROR(fmt, ...) do{ LOGE("%s " fmt, __FUNCTION__, __VA_ARGS__); }while(0)

void Scene::lua_setLightDir(LuaObject obj){
	if(obj.Get("direction").IsTable()){
		if(!Lua::luaConv(obj.Get("direction"), mDirectionalLight.mDirection)){
			// TODO handle
		}
	}
}

#if 0
uint32_t Scene::lua_createActor(){
	return createActor()->getId();
}

void Scene::lua_removeActor(uint32_t actorId){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	deleteActor(actor);
}

void Scene::lua_setActorModel(uint32_t actorId, const char* modelPath, const char* skinName){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	actor->setModel(mAssets->getModelManager()->getFromPath(modelPath), skinName);
}

void Scene::lua_playActorAnimation(uint32_t actorId, const char* animationName, bool loop){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	actor->getAnimationPlayer()->play(animationName, loop);
}

void Scene::lua_setActorPosition(uint32_t actorId, LuaObject luaPos){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	glm::vec3 pos;
	if(Lua::luaConv(luaPos, pos)){
		if(actor->getPhysicsActor() && actor->getPhysicsActor()->isControlled()){
			PxExtendedVec3 pxPos;
			pxConvert(pos, pxPos);
			actor->getPhysicsActor()->getController()->setPosition(pxPos);
		}
		else{
			actor->getTransform().setPosition(pos);
		}
	}
	else{
		LOGE("Invalid position object for actor ID %d", actorId);
	}
}

void Scene::lua_attachActor(uint32_t id1, uint32_t id2, const char* boneName){
	SceneActor* actor1 = getActorById(id1);
	if(actor1 == NULL){
		LUA_ERROR("Invalid actor ID %d", id1);
		return;
	}


	SceneActor* actor2 = getActorById(id2);
	if(actor2 == NULL){
		LUA_ERROR("Invalid actor ID %d", id2);
		return;
	}

	actor1->attach(actor2, boneName);
}

void Scene::lua_cameraLookAt(LuaObject luaPos){
	glm::vec3 pos;
	if(Lua::luaConv(luaPos, pos)){
		mCamera->lookAt(pos);
	}
	else{
		LOGE("Invalid camera position, expected vec3");
	}

}

void Scene::lua_setActorFacing(uint32_t actorId, LuaObject luaFacing){
	glm::vec3 facing;
	Lua::luaConv(luaFacing, facing);

	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	actor->getTransform().setFacing(facing, glm::vec3(0, 1, 0));
}

void Scene::lua_createActorController(uint32_t actorId, LuaObject physicsGroup, LuaObject ctrlDesc){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	PhysicsActor::Desc desc;

	if(!Lua::luaConv(physicsGroup, desc.group)){
		desc.group = 0;
	}
	
	desc.type = PhysicsActor::eDYNAMIC_ACTOR;
	desc.controlMode = PhysicsActor::eCONTROLLER_MODE;
	desc.controllerDesc.geometryType = PhysicsActor::eCAPSULE_CONTROLLER;
	desc.collisionMask = 0;

	if(ctrlDesc.IsNil()){
		desc.controllerDesc.geometryDesc.capsuleController.height = 2;
		desc.controllerDesc.geometryDesc.capsuleController.radius = 1;
	}
	else{
		Lua::luaConv(ctrlDesc.Get("radius"), desc.controllerDesc.geometryDesc.capsuleController.radius);
		Lua::luaConv(ctrlDesc.Get("height"), desc.controllerDesc.geometryDesc.capsuleController.height);
	}

	desc.pose = actor->getTransform();

	mPhysics->createActor(actor, desc);
}


class FilterCallback : public PxSceneQueryFilterCallback, public Singleton<FilterCallback>{
public:
	virtual PxSceneQueryHitType::Enum preFilter(const PxFilterData& filterData, PxShape* shape, PxSceneQueryFilterFlags& filterFlags){
		

		PxFilterData shapeFilterData = shape->getSimulationFilterData();

		PhysicsActor* actor = (PhysicsActor*)shape->getActor().userData;
		
		LOG("%d %#x %#x %#x %#x", actor->getId(), shapeFilterData.word0, shapeFilterData.word1, shapeFilterData.word2, shapeFilterData.word3);

		//if(shapeFilterData.word3 & 0x2){
		//	LOG("REGION COLLISION !");
		//	return PxSceneQueryHitType::eNONE;
		//}
		//else{
		//	//LOG("TERRAIN");
			return PxSceneQueryHitType::eTOUCH;
		//}
	}

	virtual PxSceneQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit){
		LOG("");
		return PxSceneQueryHitType::eNONE;
	}
};

void Scene::lua_moveActor(uint32_t actorId, LuaObject luaDisp, float dt){
	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	if(!actor->getPhysicsActor() || !actor->getPhysicsActor()->getController()){
		LUA_ERROR("Error moving actor (id= %d), no physics controller bound to it", actorId);
		return;
	}

	PxVec3 pxDisp;
	glm::vec3 disp;

	Lua::luaConv(luaDisp, disp);
	pxConvert(disp, pxDisp);

	// These allow customization of filtering and control >> what the character is colliding with. << ( bullshit ? )
	PxControllerFilters filters;

	PxFilterData* data = new PxFilterData;
	/*
	The PhysX filtering (successfully used for region callbacks in non-controlled 
	actors) is not applied on character controllers for some reason

	The callback might be replaced with FilterCallback : public PxSceneQueryFilterCallback but it doesn't seem to be 
	working for some reason (preFilter never fires for regions)

	We can; however, make the controller fall through the terrain by
	returning PxSceneQueryHitType::eNONE instead of PxSceneQueryHitType::eTOUCH in preFilter

	TODO investigate this further
	*/

	data->setToDefault();
	data->word0 = 0xAABBCCDD;
	filters.mFilterData = data;

	//filters.mActiveGroups = 0;

	filters.mFilterCallback = &FilterCallback::getSingleton();

	actor->getPhysicsActor()->getController()->move(pxDisp, 0.0001, dt, filters/* PxControllerFilters()*/);
}


LuaObject Scene::lua_getActorFacing(uint32_t actorId){
	LuaObject luaFacing, luaForward, luaUp;
	LUA_NEW_TABLE(luaFacing);
	LUA_NEW_TABLE(luaForward);
	LUA_NEW_TABLE(luaUp);

	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return luaFacing;
	}

	glm::vec3 forward, up;

	actor->getTransform().getFacing(forward, up);

	Lua::luaConv(forward, luaForward);
	Lua::luaConv(up, luaUp);
	
	luaFacing.Set("forward", luaForward);
	luaFacing.Set("up", luaUp);

	return luaFacing;
}

LuaObject Scene::lua_getActorTransform(uint32_t actorId){
	LuaObject luaTransform;
	LUA_NEW_TABLE(luaTransform);

	SceneActor* actor = getActorById(actorId);
	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return luaTransform;
	}

	LuaObject luaPosition;
	LUA_NEW_TABLE(luaPosition);

	Lua::luaConv(actor->getTransform().getPosition(), luaPosition);

	luaTransform.Set("position", luaPosition);
	
	return luaTransform;
}

void Scene::lua_setActorUserData(uint32_t actorId, uint32_t data){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return;
	}

	actor->setUserData((void*)data);
}

uint32_t Scene::lua_getActorUserData(uint32_t actorId){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return 0;
	}

	return (uint32_t)actor->getUserData();
}

bool Scene::lua_actorHasUserData(uint32_t actorId){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return false;
	}

	return actor->hasUserData();
}

bool Scene::lua_isActorAnimationPlaying(uint32_t actorId){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
		return false;
	}

	if(actor->getAnimationPlayer()){
		return actor->getAnimationPlayer()->isPlaying();
	}
	else{
		return false;
	}
}

void Scene::lua_destroyController(uint32_t actorId){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
	}

	if(actor->getPhysicsActor() && actor->getPhysicsActor()->getController()){
		mPhysics->removeActor( actor->getPhysicsActor() );
		actor->setPhysicsActor(NULL);
	}
}

void Scene::lua_rotateActor(uint32_t actorId, float x, float y, float z, float angle){
	SceneActor* actor = getActorById(actorId);

	if(actor == NULL){
		LUA_ERROR("Invalid actor ID %d", actorId);
	}

	actor->getTransform().rotate(x, y, z, angle);
}

#endif
void Scene::expose(LuaObject& meta){

#if 0
#define EXPOSE(funcName) do{ meta.RegisterObjectDirect(#funcName, (Scene*)0, &Scene::lua_ ## funcName); }while(0)

	EXPOSE(setLightDir);
	EXPOSE(createActor);
	EXPOSE(setActorModel);
	EXPOSE(setLightDir);
	EXPOSE(removeActor);
	EXPOSE(setActorPosition);
	EXPOSE(attachActor);
	EXPOSE(cameraLookAt);
	EXPOSE(setActorFacing);
	EXPOSE(createActorController);
	EXPOSE(moveActor);
	EXPOSE(getActorTransform);
	EXPOSE(setLightDir);
	EXPOSE(setActorUserData);
	EXPOSE(getActorUserData);
	EXPOSE(playActorAnimation);
	EXPOSE(actorHasUserData);
	EXPOSE(isActorAnimationPlaying);
	EXPOSE(destroyController);
	EXPOSE(getActorFacing);
	EXPOSE(rotateActor);
#endif
}


}; // </wt>