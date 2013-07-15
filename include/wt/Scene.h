#ifndef WT_SCENE_H
#define WT_SCENE_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/SkyBox.h"
#include "wt/Terrain.h"
#include "wt/Physics.h"
#include "wt/ASerializable.h"
#include "wt/Color.h"
#include "wt/Light.h"
#include "wt/Assets.h"
#include "wt/Fog.h"
#include "wt/ASceneActor.h"
#include "wt/ModelledActor.h"
#include "wt/ParticleEffect.h"

namespace wt{

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 3


class Renderer;



class Scene : public ALuaObject{
friend class Renderer;

public:
	typedef std::map<uint32_t, ASceneActor*> ActorMap;

	typedef std::set<ModelledActor*> ModelledActorSet;

	typedef std::set<ParticleEffect*> ParticleEffectSet;

	typedef std::set<Terrain*> TerrainSet;

protected:
	const ModelledActorSet& getModelledActors() const{
		return mModelledActors;
	}

	const TerrainSet& getTerrainSet() const{
		return mTerrainSet;
	}

	const ParticleEffectSet& getParticleEffects() const{
		return mParticleEffects;
	}

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
	
	SkyBox* mSkyBox;
	Fog mFog;
	
	Physics* mPhysics;

	DirectionalLight mDirectionalLight;

	PointLight mPointLights[MAX_POINT_LIGHTS];
	uint32_t mNumPointLights;

	SpotLight mSpotLights[MAX_SPOT_LIGHTS];
	uint32_t mNumSpotLights;
	

	Assets* mAssets;

	uint32_t generateActorId();

public:
	Scene(Physics* physics, Assets* assets);

	virtual ~Scene();

	ActorMap& getActorMap();

	void setCamera(math::Camera* camera);

	Physics* getPhysics() const;

	void deleteActor(ASceneActor* actor);

	Fog& getFog();

	void setPhysics(Physics* p);

	void addPointLight(const PointLight& light);

	void addSpotLight(const SpotLight& light);

	uint32_t getNumSpotLights() const{
		return mNumSpotLights;
	}

	SpotLight& getSpotLight(uint32_t index);

	PointLight& getPointLight(uint32_t index);

	DirectionalLight& getDirectionalLight();

	uint32_t getNumPointLights() const;
	
	ModelledActor* createModelledActor(const String& name="");

	ParticleEffect* createParticleEffect(const String& name="");

	Terrain* createTerrain(const String& name="");

	ASceneActor* findActorByName(const String& name) const;

	ASceneActor* getActorById(uint32_t id);

	void destroy();

	void update(float dt);

	math::Camera& getCamera();

	void setSkyBox(SkyBox* sky);

	SkyBox* getSkyBox();

	void clear();

	/**********************/
	/**** Lua bindings ****/
	/**********************/

	uint32_t lua_createActor();

	void lua_removeActor(uint32_t actorId);

	bool lua_isActorAnimationPlaying(uint32_t actorId);

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

	void expose(LuaObject& meta);

private:
	ActorMap::iterator eraseActor(ActorMap::iterator& iter);

}; // </Scene>

}; // </wt>


#endif
