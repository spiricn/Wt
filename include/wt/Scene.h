#ifndef WT_SCENE_H
#define WT_SCENE_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/SkyBox.h"
#include "wt/Terrain.h"
#include "wt/Physics.h"
#include "wt/ASerializable.h"
#include "wt/Color.h"
#include "wt/DirectionalLight.h"
#include "wt/SpotLight.h"
#include "wt/PointLight.h"
#include "wt/Assets.h"
#include "wt/Fog.h"
#include "wt/ASceneActor.h"
#include "wt/ModelledActor.h"
#include "wt/ParticleEffect.h"
#include "wt/EventManager.h"
#include "wt/EventEmitter.h"

namespace wt{

class Renderer;


class ARenderer;

class Scene : public lua::Object<Scene>{
friend class Renderer;
friend class ARenderer;

public:
	typedef std::map<uint32_t, ASceneActor*> ActorMap;

	typedef std::set<ModelledActor*> ModelledActorSet;

	typedef std::set<ParticleEffect*> ParticleEffectSet;

	typedef std::set<Terrain*> TerrainSet;

	typedef std::set<PointLight*> PointLightSet;

	typedef std::set<SpotLight*> SpotLightSet;

	struct GodRayParams{
		Texture2D* sourceTexture;
		glm::vec3 sourcePosition;
		Color sourceColor;
		Color rayColor;
		float sourceSize;
		float exposure;
		float decay;
		float density;
		float weight;
		uint32_t sampleNumber;
		bool enabled;
		
		GodRayParams() : sourcePosition(glm::vec3(0.347047, 0.283466, 0.893994)*1000.0f), sourceColor(Color::White()),
			rayColor(Color(0.1, 0.1, 0.1, 1.0)), sourceSize(30.0f),  exposure(0.009f),
			decay(1.0f), density(0.84f), weight(3.1f), sampleNumber(100), enabled(false), sourceTexture(NULL){
		}
	}; // </GodRayParams>

public:
	const ModelledActorSet& getModelledActors() const{
		return mModelledActors;
	}

	const TerrainSet& getTerrainSet() const{
		return mTerrainSet;
	}

	const ParticleEffectSet& getParticleEffects() const{
		return mParticleEffects;
	}
public:
	Scene(Physics* physics, Assets* assets, EventManager* eventManager, lua::State* luaState);

	virtual ~Scene();

	ActorMap& getActorMap();

	void setCamera(math::Camera* camera);

	Physics* getPhysics() const;

	void deleteActor(ASceneActor* actor);

	void getGodRayParams(GodRayParams& dst);

	void setGodRayParams(const GodRayParams& src);

	const SpotLightSet& getSpotLightSet() const;

	const PointLightSet& getPointLightSet() const;

	Fog& getFog();

	void setPhysics(Physics* p);

	const PointLight* createPointLight(const PointLight::Desc& desc);

	const SpotLight* createSpotLight(const SpotLight::Desc& desc);

	void deleteLight(const ALight* light);

	void setSpotLightDesc(const SpotLight* light, const SpotLight::Desc& desc);

	void setPointLightDesc(const PointLight* light, const PointLight::Desc& desc);

	void setDirectionalLightDesc(const DirectionalLight::Desc& desc);

	uint32_t getNumPointLights() const;

	uint32_t getNumSpotLights() const;

	uint32_t getMaxPointLights() const;

	uint32_t getMaxSpotLights() const;
	
	ModelledActor* createModelledActor(const String& name="");

	ParticleEffect* createParticleEffect(const String& name="");

	Terrain* createTerrain(const String& name="");

	ASceneActor* findActorByName(const String& name) const;

	ASceneActor* getActorById(uint32_t id);

	const DirectionalLight* getDirectionalLight() const;

	void destroy();

	void update(float dt);

	math::Camera& getCamera();

	void setSkyBox(SkyBox* sky);

	SkyBox* getSkyBox();

	void clear();

	/**********************/
	/**** Lua bindings ****/
	/**********************/
	void lua_removeActor(LuaObject obj);

	LuaObject lua_findModelledActor(const char* name);

	LuaObject lua_findParticleEffect(const char* name);

	void generateMetaTable();

	uint32_t lua_createActor();

	//void lua_removeActor(uint32_t actorId);

	/*bool lua_isActorAnimationPlaying(uint32_t actorId);

	void lua_setActorPosition(uint32_t actorId, LuaObject pos);

	void lua_setActorFacing(uint32_t actorId, LuaObject facing);

	void lua_destroyController(uint32_t actorId);

	void lua_setLightDir(LuaObject obj);

	void lua_attachActor(uint32_t actor1, uint32_t actor2, const char* boneName);

	void lua_playActorAnimation(uint32_t actorId, const char* animationName, bool loop);

	void lua_cameraLookAt(LuaObject pos);

	bool lua_actorHasUserData(uint32_t actorId);

	void lua_createActorController(uint32_t actorId, LuaObject physicsGroup, LuaObject ctrlDesc);

	void lua_moveActor(uint32_t actorId, LuaObject luaDisp, float dt);

	void lua_rotateActor(uint32_t actorId, float x, float y, float z, float angle);

	void lua_setActorUserData(uint32_t actorId, uint32_t data);

	uint32_t lua_getActorUserData(uint32_t actorId);

	LuaObject lua_getActorTransform(uint32_t actorId);

	LuaObject lua_getActorFacing(uint32_t actorId);

	void lua_setActorModel(uint32_t actorId, const char* modelPath, const char* skinName);
*/

private:
	ActorMap::iterator eraseActor(ActorMap::iterator& iter);


private:
	/** Master actor list */
	ActorMap mActors;

	/** A list of modelled actors (also contained in 'mActors') */
	ModelledActorSet mModelledActors;

	/** A list of terrain entities (also contained in 'mActors') */
	TerrainSet mTerrainSet;

	ParticleEffectSet mParticleEffects;

	math::Camera* mCamera;

	math::Camera mDefaultCamera;

	SpotLight* findSpotLight(uint32_t id);

	PointLight* findPointLight(uint32_t id);
	
	SkyBox* mSkyBox;
	Fog mFog;
	
	Physics* mPhysics;

	Sp<DirectionalLight> mDirectionalLight;
	
	EventManager* mEventManager;

	SpotLightSet mSpotLightSet;

	typedef std::map<uint32_t, SpotLight*> SpotLightMap;
	SpotLightMap mSpotLights;

	typedef std::map<uint32_t, PointLight*> PointLightMap;
	PointLightMap mPointLights;

	PointLightSet mPointLightSet;

	lua::State* mLuaState;

	Assets* mAssets;

	uint32_t generateActorId();

	GodRayParams mGodrayParams;

	EventEmitter mEventEmitter;

	void onLightingModified(ALight* light);

	void onLightCreated(ALight* light);

	void onLightDeleted(ALight* light);

	// Lighting

}; // </Scene>

class SceneLightUpdated : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	enum Type{
		eTYPE_CREATED,
		eTYPE_MODIFIED,
	}; // </Type>

	const ALight* light;

	Type type;

	SceneLightUpdated(Type type, const ALight* light) : light(light), type(type){
	}

	SceneLightUpdated(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </SceneLightUpdated>


class SceneLightDeleted : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	Sp<const ALight> light;

	SceneLightDeleted(Sp<const ALight> light) : light(light){
	}

	SceneLightDeleted(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </SceneLightingModifiedEvent>

}; // </wt>


#endif
