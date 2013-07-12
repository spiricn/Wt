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

namespace wt{

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 3

class Scene : public ALuaObject{
public:
	typedef std::map<Uint32, SceneActor*> ActorMap;


private:
	static const char* TAG;

	math::Camera* mCamera;
	math::Camera mDefaultCamera;
	ActorMap mActors;
	SkyBox* mSkyBox;
	Fog mFog;
	
	Physics* mPhysics;
	Terrain* mTerrain;

	DirectionalLight mDirectionalLight;

	PointLight mPointLights[MAX_POINT_LIGHTS];
	Uint32 mNumPointLights;

	SpotLight mSpotLights[MAX_SPOT_LIGHTS];
	Uint32 mNumSpotLights;
	

	Assets* mAssets;

	Uint32 generateActorId();

public:
	Scene(Physics* physics, Assets* assets);

	virtual ~Scene();

	ActorMap& getActorMap();

	void setCamera(math::Camera* camera);

	Terrain* getTerrain();

	Physics* getPhysics() const;

	void deleteActor(SceneActor* actor){
		mActors.erase( actor->getId() );
		if(actor->getPhysicsActor()){
			mPhysics->removeActor( actor->getPhysicsActor() );
		}

		delete actor;
	}

	Fog& getFog();

	void createTerrain(const LuaObject& config);

	void createTerrain(const TerrainDesc& tDesc);

	void destroyTerrain();

	void setPhysics(Physics* p);

	void addPointLight(const PointLight& light);

	void addSpotLight(const SpotLight& light);

	Uint32 getNumSpotLights() const{
		return mNumSpotLights;
	}

	SpotLight& getSpotLight(Uint32 index);

	PointLight& getPointLight(Uint32 index);

	DirectionalLight& getDirectionalLight();

	Uint32 getNumPointLights() const;
	
	SceneActor* createActor(const String& name="");

	SceneActor* findActorByName(const String& name) const;

	SceneActor* getActorById(Uint32 id);

	void destroy();

	void update(float dt);

	math::Camera& getCamera();

	void setSkyBox(SkyBox* sky);

	SkyBox* getSkyBox();

	void clear();

	/**********************/
	/**** Lua bindings ****/
	/**********************/

	Uint32 lua_createActor();

	void lua_removeActor(Uint32 actorId);

	bool lua_isActorAnimationPlaying(Uint32 actorId);

	void lua_setActorPosition(Uint32 actorId, LuaObject pos);

	void lua_setActorFacing(Uint32 actorId, LuaObject facing);

	void lua_destroyController(Uint32 actorId);

	void lua_setLightDir(LuaObject obj);

	void lua_attachActor(Uint32 actor1, Uint32 actor2, const char* boneName);

	void lua_playActorAnimation(Uint32 actorId, const char* animationName, bool loop);

	void lua_cameraLookAt(LuaObject pos);

	bool lua_actorHasUserData(Uint32 actorId);

	void lua_createActorController(Uint32 actorId, LuaObject physicsGroup, LuaObject ctrlDesc);

	void lua_moveActor(Uint32 actorId, LuaObject luaDisp, float dt);

	void lua_rotateActor(Uint32 actorId, float x, float y, float z, float angle);

	void lua_setActorUserData(Uint32 actorId, Uint32 data);

	Uint32 lua_getActorUserData(Uint32 actorId);

	LuaObject lua_getActorTransform(Uint32 actorId);

	LuaObject lua_getActorFacing(Uint32 actorId);

	void lua_setActorModel(Uint32 actorId, const char* modelPath, const char* skinName);

	void expose(LuaObject& meta);

}; // </Scene>

}; // </wt>


#endif
