#ifndef WT_SCENE_H
#define WT_SCENE_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/Physics.h"
#include "wt/ASerializable.h"
#include "wt/Color.h"
#include "wt/DirectionalLight.h"
#include "wt/SpotLight.h"
#include "wt/PointLight.h"
#include "wt/Assets.h"
#include "wt/Fog.h"
#include "wt/ASceneActor.h"
#include "wt/EventManager.h"
#include "wt/EventEmitter.h"

namespace wt
{

namespace gui
{
	class Window;
} // </gui>

class Renderer;
class ARenderer;
class Sound;
class ColliderActor;
class ModelledActor;
class ParticleEffect;
class SkyBox;
class Terrain;

class Scene{
public:
	typedef std::map<uint32_t, ASceneActor*> ActorMap;

	typedef std::set<ASceneActor*> ActorSet;

	typedef std::map<ASceneActor::ActorType, ActorSet*> ActorSetMap;

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

	struct ShadowMappingDesc{
		bool enabled;
		Camera casterSource;
		float shadowIntensity;

		ShadowMappingDesc();
	}; // </ShadowMappingDesc>

	class IListener{
	public:
		virtual void onSceneShadowMappingParamsChanged(Scene* scene, const ShadowMappingDesc& desc){
		}

		virtual void onSceneFogParamsChanged(Scene* scene, const FogDesc& desc){
		}

		virtual void onSceneLightUpdated(Scene* scene, const ALight& light){
		}

		virtual void onSceneLightCreated(Scene* scene, const ALight& light){
		}

		virtual void onSceneLightDeleted(Scene* scene, const ALight& light){
		}

	}; // </IListener>

public:
	Scene(Physics* physics, AResourceSystem* assets, EventManager* eventManager, lua::State* luaState);

	virtual ~Scene();

	const ShadowMappingDesc& getShadowMappingDesc() const;

	void setShadowMappingDesc(const ShadowMappingDesc& desc);

	ActorMap& getActorMap();

	void setCamera(Camera* camera);

	Physics* getPhysics() const;

	gui::Window* getUIWindow();

	void deleteActor(ASceneActor* actor);

	void getGodRayParams(GodRayParams& dst);

	void setGodRayParams(const GodRayParams& src);

	const FogDesc& getFogDesc();

	const void setFogDesc(const FogDesc& desc);

	void setPhysics(Physics* p);

	const PointLight* createPointLight(const String& name="", const PointLight::Desc& desc=PointLight::Desc());

	const SpotLight* createSpotLight(const SpotLight::Desc& desc);

	void deleteLight(const ALight* light);

	ASceneActor* createActor(ASceneActor::ActorType type, const String&  name="");

	void setSpotLightDesc(const SpotLight* light, const SpotLight::Desc& desc);

	void setPointLightDesc(const PointLight* light, const PointLight::Desc& desc);

	void setDirectionalLightDesc(const DirectionalLight::Desc& desc);

	Sound* createSound(const String& name);

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

	Camera& getCamera();

	void setSkyBox(SkyBox* sky);

	SkyBox* getSkyBox();

	void clear();

	AResourceSystem* getAssets();

	void setUIWindow(gui::Window* window);

	void registerListener(IListener* listener);

	void unregisterListener(IListener* listener);

	// TODO make this private
	ActorSet& getActorSet(ASceneActor::ActorType type);

	void insertCustomActor(ASceneActor* actor);

	ColliderActor* createColliderActor(const String& name="");

	const ActorSet& getActorSet(ASceneActor::ActorType type) const;

private:
	typedef std::vector<IListener*> ListenerList;

	typedef std::map<uint32_t, SpotLight*> SpotLightMap;

private:

	template<class T>
	void setErase(T* actor){
		ActorSet& set = getActorSet(actor->getActorType());

		set.erase(dynamic_cast<ASceneActor*>(actor));
	}

	template<class T>
	void setInsert(T* actor){
		ActorSet& set = getActorSet(actor->getActorType());

		set.insert(dynamic_cast<ASceneActor*>(actor));
	}

	uint32_t generateActorId();

	void onLightingModified(ALight* light);

	void onLightCreated(ALight* light);

	void onLightDeleted(ALight* light);

	ActorMap::iterator eraseActor(ActorMap::iterator& iter);

	SpotLight* findSpotLight(uint32_t id);

	PointLight* findPointLight(uint32_t id);

private:
	GodRayParams mGodrayParams;
	SpotLightMap mSpotLights;
	lua::State* mLuaState;
	AResourceSystem* mAssets;
	SkyBox* mSkyBox;
	FogDesc mFogDesc;
	Physics* mPhysics;
	gui::Window* mUIWindow;
	Sp<DirectionalLight> mDirectionalLight;
	EventManager* mEventManager;
	ActorMap mActors;
	ActorSetMap mActorSets;
	Camera* mCamera;
	ShadowMappingDesc mShadowMapping;
	ListenerList mListeners;
	Camera mDefaultCamera;

private:
friend class Renderer;
friend class ARenderer;

}; // </Scene>

}; // </wt>


#endif
