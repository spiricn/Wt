
#include "wt/stdafx.h"

#include "wt/Scene.h"

#define TD_TRACE_TAG "Scene"

#define MAX_POINT_LIGHTS 10

#define MAX_SPOT_LIGHTS 10

namespace wt{

Scene::Scene(Physics* physics, Assets* assets, EventManager* eventManager, lua::State* luaState) :  mSkyBox(NULL), mEventManager(eventManager),
	mAssets(assets), mPhysics(physics), mLuaState(luaState), mDirectionalLight(NULL){

	setCamera(&mDefaultCamera);

	mEventEmitter.hook(mEventManager,
		2,
		SceneLightUpdated::TYPE,
		SceneLightDeleted::TYPE
	);

	mDirectionalLight = new DirectionalLight(this, 0);
}

const DirectionalLight* Scene::getDirectionalLight() const{
	return mDirectionalLight;
}

void Scene::onLightingModified(ALight* light){
	mEventManager->queueEvent(
		new SceneLightUpdated(SceneLightUpdated::eTYPE_MODIFIED, light)
	);
}

void Scene::onLightCreated(ALight* light){
	mEventManager->queueEvent(
		new SceneLightUpdated(SceneLightUpdated::eTYPE_CREATED, light)
	);
}

void Scene::onLightDeleted(ALight* light){
	mEventManager->queueEvent(
		new SceneLightDeleted(light)
	);
}

const Scene::SpotLightSet& Scene::getSpotLightSet() const{
	return mSpotLightSet;
}

const Scene::PointLightSet& Scene::getPointLightSet() const{
	return mPointLightSet;
}

void Scene::deleteLight(const ALight* alight){
	if(alight->getType() == ALight::eTYPE_SPOT){
		SpotLight* light = (SpotLight*)alight;	

		mSpotLights.erase(light->getId());
		mSpotLightSet.erase(light);
	}
	else{
		WT_THROW("Sanity check fail");
	}

	onLightDeleted((ALight*)alight);
}

const SpotLight* Scene::createSpotLight(const SpotLight::Desc& desc){
	uint32_t id = 0;

	while(true){
		if(findSpotLight(id) != NULL){
			++id;
		}
		else{
			break;
		}
	}

	SpotLight* light = new SpotLight(this, id);

	light->getDesc() = desc;

	mSpotLights.insert(std::make_pair(id, light));
	mSpotLightSet.insert(light);

	onLightCreated(light);

	return light;
}

void Scene::getGodRayParams(GodRayParams& dst){
	dst = mGodrayParams;
}

void Scene::setGodRayParams(const GodRayParams& src){
	mGodrayParams = src;
}

SpotLight* Scene::findSpotLight(uint32_t id){
	SpotLightMap::iterator iter = mSpotLights.find(id);
	return iter == mSpotLights.end() ? NULL : iter->second;
}

PointLight* Scene::findPointLight(uint32_t id){
	ASceneActor* actor = getActorById(id);
	if(actor->getActorType() == ASceneActor::eTYPE_POINT_LIGHT){
		return dynamic_cast<PointLight*>(actor);
	}
	else{
		return NULL;
	}
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

void Scene::setSpotLightDesc(const SpotLight* clight, const SpotLight::Desc& desc){
	// Safe
	SpotLight* light = const_cast<SpotLight*>(clight);

	light->getDesc() = desc;

	onLightingModified(const_cast<SpotLight*>(light));
}

void Scene::setPointLightDesc(const PointLight* clight, const PointLight::Desc& desc){
	// Safe
	PointLight* light = const_cast<PointLight*>(clight);

	light->getDesc() = desc;

	onLightingModified(const_cast<PointLight*>(light));
}

void Scene::setDirectionalLightDesc(const DirectionalLight::Desc& desc){
	mDirectionalLight->getDesc() = desc;

	onLightingModified(mDirectionalLight);
}

const PointLight* Scene::createPointLight(const PointLight::Desc& desc){
	uint32_t id = 0;

	PointLight* light = new PointLight(this, generateActorId(), "");

	light->setLuaState(mLuaState);

	mActors.insert( std::make_pair(((ASceneActor*)light)->getId(), light) );
	
	light->getDesc() = desc;

	mPointLightSet.insert(light);

	onLightCreated(light);

	return light;
}

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

		delete iter->second;
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_TERRAIN){
		WT_ASSERT(mTerrainSet.erase( static_cast<Terrain*>(iter->second) ) == 1, "Error erasing actor");

		delete iter->second;
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_PARTICLE_EFFECT){
		WT_ASSERT(mParticleEffects.erase( static_cast<ParticleEffect*>(iter->second) ), "Error erasing actor");

		delete iter->second;
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_POINT_LIGHT){
		PointLight* light = static_cast<PointLight*>(iter->second);

		mPointLightSet.erase(light);

		// We don't delete the object directly but rather let a smart pointer do it
		onLightDeleted((ALight*)light);
	}
	else if(iter->second->getActorType() == ASceneActor::eTYPE_SOUND){
		Sound* sound = dynamic_cast<Sound*>(iter->second);

		mSoundSet.erase(sound);
	}
	else{
		WT_THROW("Unhandled actor type");
	}

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

	effect->setLuaState(mLuaState);

	return effect;
}

ModelledActor* Scene::createModelledActor(const String& name){
	ModelledActor* actor = new ModelledActor(this, generateActorId(), name);

	actor->setLuaState(mLuaState);

	mActors.insert( std::make_pair(actor->getId(), actor) );

	mModelledActors.insert(actor);

	return actor;
}

Sound* Scene::createSound(const String& name){
	Sound* sound = new Sound(this, generateActorId(), name);

	sound->setLuaState(mLuaState);

	mActors.insert( std::make_pair(sound->getId(), sound) );

	mSoundSet.insert(sound);

	return sound;
}

ASceneActor* Scene::findActorByName(const String& name) const{
	for(ActorMap::const_iterator i=mActors.cbegin(); i!=mActors.cend(); i++){
		if(i->second->getName().compare(name) == 0){
			return i->second;
		}
	}

	return NULL;
}

uint32_t Scene::getNumPointLights() const{
	return mPointLightSet.size();
}

uint32_t Scene::getNumSpotLights() const{
	return mSpotLights.size();
}

uint32_t Scene::getMaxPointLights() const{
	return MAX_POINT_LIGHTS;
}

uint32_t Scene::getMaxSpotLights() const{
	return MAX_SPOT_LIGHTS;
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

const EvtType SceneLightUpdated::TYPE = "SceneLightUpdated";

const EvtType SceneLightDeleted::TYPE = "SceneLightDeleted";

}; // </wt>