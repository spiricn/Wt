
#include "wt/stdafx.h"

#include "wt/Scene.h"

#define TD_TRACE_TAG "Scene"

#define MAX_POINT_LIGHTS 10

#define MAX_SPOT_LIGHTS 10

namespace wt{

Scene::Scene(Physics* physics, AResourceSystem* assets, EventManager* eventManager, lua::State* luaState) :  mSkyBox(NULL), mEventManager(eventManager),
	mAssets(assets), mPhysics(physics), mLuaState(luaState), mDirectionalLight(NULL), mUIWindow(NULL){

	setCamera(&mDefaultCamera);

	mDirectionalLight = new DirectionalLight(this, 0);

	// Create an actor set for each actor type
	for(int i=0; i<ASceneActor::eTYPE_MAX; i++){
		mActorSets.insert(std::make_pair(static_cast<ASceneActor::ActorType>(i), new ActorSet));
	}
}

void Scene::insertCustomActor(ASceneActor* actor){

	actor->setLuaState(mLuaState);

	actor->setId( generateActorId() );

	mActors.insert( std::make_pair(actor->getId(), actor) );

	setInsert(actor);
}

ASceneActor* Scene::createActor(ASceneActor::ActorType type, const String&  name){
	switch(type){
	case ASceneActor::eTYPE_MODELLED:
		return dynamic_cast<ASceneActor*>(createModelledActor(name));
	case ASceneActor::eTYPE_PARTICLE_EFFECT:
		return dynamic_cast<ASceneActor*>(createParticleEffect(name));
	case ASceneActor::eTYPE_POINT_LIGHT:
		return dynamic_cast<ASceneActor*>(const_cast<PointLight*>(createPointLight(name)));
	case ASceneActor::eTYPE_SOUND:
		return dynamic_cast<ASceneActor*>(createSound(name));
	case ASceneActor::eTYPE_TERRAIN:
		return dynamic_cast<ASceneActor*>(createTerrain(name));
	default:
		WT_THROW("Unhandled actor type %d", type);
	}
}


gui::Window* Scene::getUIWindow(){
	return mUIWindow;
}

void Scene::setUIWindow(gui::Window* window){
	mUIWindow = window;
}

void Scene::registerListener(IListener* listener){
	ListenerList::iterator iter = std::find(mListeners.begin(), mListeners.end(), listener);
	WT_ASSERT(iter==mListeners.end(), "Listener %p already registered");

	mListeners.push_back(listener);
}

void Scene::unregisterListener(IListener* listener){
	ListenerList::iterator iter = std::find(mListeners.begin(), mListeners.end(), listener);
	WT_ASSERT(iter!=mListeners.end(), "Listener not registered");

	mListeners.erase(iter);
}

const DirectionalLight* Scene::getDirectionalLight() const{
	return mDirectionalLight;
}

void Scene::onLightingModified(ALight* light){
	// Notify listeners
	for(ListenerList::iterator iter=mListeners.begin(); iter!=mListeners.end(); iter++){
		(*iter)->onSceneLightUpdated(this, *light);
	}
}

void Scene::onLightCreated(ALight* light){
	// Notify listeners
	for(ListenerList::iterator iter=mListeners.begin(); iter!=mListeners.end(); iter++){
		(*iter)->onSceneLightCreated(this, *light);
	}
}

void Scene::onLightDeleted(ALight* light){
	// Notify listeners
	for(ListenerList::iterator iter=mListeners.begin(); iter!=mListeners.end(); iter++){
		(*iter)->onSceneLightDeleted(this, *light);
	}
}

void Scene::deleteLight(const ALight* alight){
	if(alight->getType() == ALight::eTYPE_SPOT){
#if 0
		SpotLight* light = (SpotLight*)alight;	

		mSpotLights.erase(light->getId());

		setErase(const_cast<ALight*>(alight));
#else
		TRACEW("NOT IMPLEMENTED");
#endif
	}
	else{
		WT_THROW("Sanity check fail");
	}

	onLightDeleted((ALight*)alight);
}

		
Scene::ShadowMappingDesc::ShadowMappingDesc() : enabled(true){
}

const Scene::ShadowMappingDesc& Scene::getShadowMappingDesc() const{
	return mShadowMapping;
}

void Scene::setShadowMappingDesc(const ShadowMappingDesc& desc){
	mShadowMapping = desc;

	// Notify listeners
	for(ListenerList::iterator iter=mListeners.begin(); iter!=mListeners.end(); iter++){
		(*iter)->onSceneShadowMappingParamsChanged(this, desc);
	}
}


const SpotLight* Scene::createSpotLight(const SpotLight::Desc& desc){
#if 0
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
	
	setInsert(light);

	onLightCreated(light);

	return light;
#else
	TRACEW("NOT IMPLEMENTED");

	return NULL;
#endif
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

const FogDesc& Scene::getFogDesc(){
	return mFogDesc;
}

const void Scene::setFogDesc(const FogDesc& desc){
	mFogDesc = desc;

	// Notify listeners
	for(ListenerList::iterator iter=mListeners.begin(); iter!=mListeners.end(); iter++){
		(*iter)->onSceneFogParamsChanged(this, mFogDesc);
	}
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

const PointLight* Scene::createPointLight(const String& name, const PointLight::Desc& desc){
	uint32_t id = 0;

	PointLight* light = new PointLight(this, generateActorId(), name);

	light->setLuaState(mLuaState);

	mActors.insert( std::make_pair(((ASceneActor*)light)->getId(), light) );
	
	light->getDesc() = desc;

	setInsert(light);

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

	setErase(iter->second);

	if(iter->second->getActorType() == ASceneActor::eTYPE_POINT_LIGHT){
		PointLight* light = static_cast<PointLight*>(iter->second);

		// We don't delete the object directly but rather let a smart pointer do it
		onLightDeleted((ALight*)light);
	}
	// TODO sound?
	else{
		delete iter->second;
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

	setInsert(terrain);

	return terrain;
}

ParticleEffect* Scene::createParticleEffect(const String& name){
	ParticleEffect* effect = new ParticleEffect(this, generateActorId(), name);

	mActors.insert( std::make_pair(effect->getId(), effect) );

	setInsert(effect);

	effect->setLuaState(mLuaState);

	return effect;
}

ModelledActor* Scene::createModelledActor(const String& name){
	ModelledActor* actor = new ModelledActor(this, generateActorId(), name);

	actor->setLuaState(mLuaState);

	mActors.insert( std::make_pair(actor->getId(), actor) );

	setInsert(actor);

	return actor;
}

Sound* Scene::createSound(const String& name){
	Sound* sound = new Sound(this, generateActorId(), name);

	sound->setLuaState(mLuaState);

	mActors.insert( std::make_pair(sound->getId(), sound) );

	setInsert(sound);

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
	return getActorSet(ASceneActor::eTYPE_POINT_LIGHT).size();
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

}; // </wt>