#ifndef WT_PHYSICS_H
#define WT_PHYSICS_H

#include "wt/stdafx.h"
#include "wt/EventManager.h"
#include "wt/ASceneActor.h"
#include "wt/ASerializable.h"
#include "wt/PhysicsActor.h"
#include "wt/TimeAccumulator.h"
#include "wt/EventEmitter.h"
#include "wt/PxWt.h"

using namespace physx;

namespace wt
{

namespace math
{
	class Frustum;
	class Camera;
} // </math>


class Physics;

class RaycastHitEvent;

class RegionEvent;

class Physics : public PxQueryFilterCallback, public PxErrorCallback, public PxSimulationEventCallback{
public:
	typedef std::map<uint32_t, Sp<PhysicsActor>> ActorMap;
	
	enum PickFlag{
		ePICK_TERRAIN = 1 << 1,
		ePICK_BOUNDING_BOXES = 1 << 2,
		ePICK_ACTORS = 1 << 3
	}; // </PickFlags>

	typedef std::set<PhysicsActor*> ActorSet;

public:
	Physics(EventManager* eventManager);

    virtual ~Physics();

	Sp<PxGeometry> createGeometry(const PhysicsActor::Desc& desc);

	PxController* createController(const PhysicsActor::Desc& desc);

	PxTriangleMesh* cook(PxBoundedData& vertices, PxBoundedData& indices);

    PxTriangleMesh* cook(gl::Batch& src);

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

	bool pick(const glm::vec3& origin, const glm::vec3& direction, RaycastHitEvent& res, uint32_t groups=0xFFFFFFFF, PickFlag flags=ePICK_ACTORS);

	bool pick(Camera& camera, const glm::vec2& screenPos, const glm::vec2& screenSize, RaycastHitEvent& res, uint32_t groups=0xFFFFFFFF, PickFlag flags=ePICK_ACTORS);

	const ActorSet& getRegionSet() const;

	void deleteRegion(uint32_t region);

private:
	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line);

	static PxFilterFlags filterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

	PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);

	PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit);

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

	void onTrigger(PxTriggerPair* pairs, PxU32 count);

	void onConstraintBreak(PxConstraintInfo*, PxU32);

	void onWake(PxActor** , PxU32 );

	void onSleep(PxActor** , PxU32 );

private:
    PxPhysics* mSdk;
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
	ActorMap mActors;
	TimeAccumulator mTimeAccumulator;
	ActorSet mRegions;
}; // </Physics>

Physics::PickFlag operator|(Physics::PickFlag a, Physics::PickFlag b);

class RaycastHitEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;
	PhysicsActor* mPickedActor;
	float mDistance;
	uint32_t mTriangleIndex;
	glm::vec3 mImpact;

	RaycastHitEvent(PhysicsActor* pickedActor=NULL, float distance=0, uint32_t faceIndex=0, 
		const glm::vec3& impact=glm::vec3(0,0,0)) : mPickedActor(pickedActor), mDistance(distance),
		mTriangleIndex(faceIndex), mImpact(impact){
	}

	const EventType& getType() const {
		return TYPE;
	}

}; // </RaycastHitEvent>


class RegionEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
		dst.Set("region", static_cast<void*>(regionActor));
		dst.Set("collidingActor", static_cast<void*>(collidingActor));
		dst.Set("type", type == eACTOR_LEFT_REGION ? "left" : "entered");
	}

	void deserialize(LuaObject& /*src*/){
	}

public:
	enum Type{
		eACTOR_LEFT_REGION,
		eACTOR_ENTERED_REGION
	};

	PhysicsActor* regionActor;
	PhysicsActor* collidingActor;

	Type type;

	static const EventType TYPE;

	RegionEvent(PhysicsActor* regionActor, PhysicsActor* collidingActor, Type type) : type(type), regionActor(regionActor), collidingActor(collidingActor){
	}

	const EventType& getType() const{
		return TYPE;
	}

}; // </ActorEnteredRegionEvent>

}; // </wt>

#endif
