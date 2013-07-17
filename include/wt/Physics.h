#ifndef WT_PHYSICS_H
#define WT_PHYSICS_H


#include "wt/stdafx.h"

#include "wt/EventManager.h"
#include "wt/ASceneActor.h"
#include "wt/ASerializable.h"
#include "wt/PhysicsActor.h"
#include "wt/TimeAccumulator.h"
#include "wt/EventEmitter.h"

using namespace physx;

namespace wt{

namespace math{
	class Frustum;
	class Camera;
}


void pxConvert(const glm::vec3& src, physx::PxVec3& dst);

void pxConvert(const glm::vec3& src, physx::PxExtendedVec3& dst);

void pxConvert(const physx::PxExtendedVec3& src, glm::vec3& dst);

void pxConvert(const PxTransform& src, math::Transform& dst);

void pxConvert(const math::Transform& src, PxTransform& dst);

class RaycastHitEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;
	PhysicsActor* mPickedActor;
	float mDistance;
	uint32_t mTriangleIndex;
	glm::vec3 mImpact;

	RaycastHitEvent(PhysicsActor* pickedActor=NULL, float distance=0, uint32_t faceIndex=0, 
		const glm::vec3& impact=glm::vec3(0,0,0)) : mPickedActor(pickedActor), mDistance(distance),
		mTriangleIndex(faceIndex), mImpact(impact){
	}

	const EvtType& getType() const {
		return TYPE;
	}

}; // </RaycastHitEvent>

class WtPxErrorCallback : public PxErrorCallback{
	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line){
		if( code != 128 /* static actor moved (safe to ignore)*/
			){
			TRACEE("Error (code %d): \"%s\" at %s:%d", code, message, file, line);
		}
	}
}; // </WtPxErrorCallback>

class Physics;

class RegionEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.Set("region", regionId);
		dst.Set("actor", actor->getId());
		dst.Set("type", type);
	}

	void deserialize(LuaObject& /*src*/){

	}

public:
	enum Type{
		eACTOR_LEFT_REGION,
		eACTOR_ENTERED_REGION
	};

	uint32_t regionId;
	PhysicsActor* actor;
	Type type;

	static const EvtType TYPE;

	RegionEvent(PhysicsActor* actor, uint32_t regionId, Type type) : actor(actor), regionId(regionId), type(type){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </ActorEnteredRegionEvent>

class Physics : public ALuaObject{
private:
    const String TAG;

    PxPhysics* mSdk;
    WtPxErrorCallback mDefaultErrorCallback;
    PxDefaultAllocator mDefaultAllocator;
    PxDefaultCpuDispatcher* mCpuDispatcher;
    PxScene* mScene;
    PxFoundation* mFoundation;
    PxSimulationFilterShader mDefaultFilterShader;
    PxMaterial* mDefaultMaterial;
    PxControllerManager* mCtrlManager;
    PxCooking* mCooking;
	EventManager* mEventManager;
	EventEmitter mEventEmitter;

	TimeAccumulator mTimeAccumulator;

    static void convertTransform(const PxTransform& src, math::Transform& dst);

	static void convertTransform(const math::Transform& src, PxTransform& dst);

	Sp<PxGeometry> createGeometry(const PhysicsActor::Desc& desc);

	PxController* createController(const PhysicsActor::Desc& desc);

	typedef std::map<uint32_t, Sp<PhysicsActor>> ActorMap;

	ActorMap mActors;

	PxTriangleMesh* cook(PxBoundedData& vertices, PxBoundedData& indices);

    PxTriangleMesh* cook(Gl::Batch& src);

	static PxFilterFlags filterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

public:
	Physics(EventManager* eventManager);

    virtual ~Physics();

	uint32_t createRegion(const String& name, const glm::vec3& position, float radius);

	void setTimeStep(float step);

	Sp<Buffer<PhysicsActor*>> getActorsInRegion(const glm::vec3& pos, float radius, uint32_t groups=0x00);

	uint32_t generateActorId();

	PhysicsActor* getActorById(uint32_t id);

	PhysicsActor* createActor(ASceneActor* sceneActor, PhysicsActor::Desc& desc);

    void update(float dt);

	PxMaterial& getDefaultMaterial();

	bool connectToVisualDebugger(const String& addr, int32_t port, int32_t timeout=100);

	PxPhysics* getSdk();

	PxScene* getScene();

	float getTerrainHeightAt(const glm::vec2& pos);

	void removeActor(PhysicsActor* actor);

	void createBBox(ASceneActor* actor);

	enum PickFlag{
		ePICK_TERRAIN = 1 << 1,
		ePICK_BOUNDING_BOXES = 1 << 2,
		ePICK_ACTORS = 1 << 3

	}; // </PickFlags>

	bool pick(const glm::vec3& origin, const glm::vec3& direction, RaycastHitEvent& res, uint32_t groups=0x00, PickFlag flags=ePICK_ACTORS);

	bool pick(math::Camera& camera, const math::Frustum& frustum, const glm::vec2& screenPos, const glm::vec2& screenSize, RaycastHitEvent& res, uint32_t groups=0x00, PickFlag flags=ePICK_ACTORS);

	/**********************/
	/**** Lua bindings ****/
	/**********************/
	LuaObject lua_getActorsInRegion(LuaObject luaPos, LuaObject luaRadius, LuaObject groupFilter);

	int32_t lua_createRegion(LuaObject luaPos, LuaObject luaRadius);

	void expose(LuaObject& meta);

}; // </Physics>

Physics::PickFlag operator|(Physics::PickFlag a, Physics::PickFlag b);

}; // </wt>

#endif
