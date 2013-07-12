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
			LOGE("PhysX", "Error (code %d): \"%s\" at %s:%d", code, message, file, line);
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
	
    virtual ~Physics();

    Physics(EventManager* eventManager);

	uint32_t createRegion(const String& name, const glm::vec3& position, float radius);

	void setTimeStep(float step){
		mTimeAccumulator.setStep(step);
	}

	Sp<Buffer<PhysicsActor*>> getActorsInRegion(const glm::vec3& pos, float radius, uint32_t groups=0x00){
		PxSphereGeometry sphere(radius);
		PxTransform pose = PxTransform::createIdentity();
		pxConvert(pos, pose.p);

		const PxU32 bufferSize = 256;
		PxShape* hitBuffer[bufferSize];

		PxSceneQueryFilterData filterData;
		filterData.data.setToDefault();
		filterData.data.word0 = groups;

		PxI32 hitNum = mScene->overlapMultiple(sphere, pose, hitBuffer, bufferSize, filterData);

		if(hitNum == -1){
			// TODO increase buffer size dynamically?
			LOGW("Buffer overflow (more than %d actors contained in radius)", bufferSize);
			return NULL;
		}
		else{
			Buffer<PhysicsActor*>* bfr = new Buffer<PhysicsActor*>();
			bfr->create(hitNum);
			for(int32_t i=0; i<hitNum; i++){
				if(hitBuffer[i]->getActor().userData){
					bfr->put(
						static_cast<PhysicsActor*>(hitBuffer[i]->getActor().userData)
						);
				}
				else{
					LOGW("No user data found for PxActor!");
				}
			}

			return bfr;
		}
	}

	uint32_t generateActorId(){
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

	PhysicsActor* getActorById(uint32_t id){
		ActorMap::iterator i = mActors.find(id);
		return i == mActors.end() ? NULL : i->second;
	}

	PhysicsActor* createActor(ASceneActor* sceneActor, PhysicsActor::Desc& desc);

    void update(float dt);

	PxMaterial& getDefaultMaterial();

	void connectToVisualDebugger(const String& addr, int32_t port, int32_t timeout=100);

	PxPhysics* getSdk();

	PxScene* getScene();

	float getTerrainHeightAt(const glm::vec2& pos);

	void removeActor(PhysicsActor* actor);

	//PxDistanceJoint* 
	bool pick(const glm::vec3& origin, const glm::vec3& direction, RaycastHitEvent& res, uint32_t groups=0x00);

	bool pick(math::Camera& camera, const math::Frustum& frustum, const glm::vec2& screenPos, const glm::vec2& screenSize, RaycastHitEvent& res, uint32_t groups=0x00);

	/**********************/
	/**** Lua bindings ****/
	/**********************/
	LuaObject lua_getActorsInRegion(LuaObject luaPos, LuaObject luaRadius, LuaObject groupFilter){
		float radius;
		glm::vec3 pos;

		LuaObject res;
		LUA_NEW_TABLE(res);

		// TODO checks
		Lua::luaConv(luaPos, pos);
		Lua::luaConv(luaRadius, radius);

		Sp<Buffer<PhysicsActor*>> actors = getActorsInRegion(pos, radius, groupFilter.IsNil() ? 0x00 : groupFilter.ToInteger() );

		uint32_t actorIdx=0;
		for(uint32_t i=0; i<actors->getCapacity(); i++){
			ASceneActor* actor =  actors->operator[](i)->getSceneActor();
			if(actor){
				res.Set(++actorIdx, actor->getId());
			}
		}

		return res;
	}

	int32_t lua_createRegion(LuaObject luaPos, LuaObject luaRadius){
		glm::vec3 pos;
		float radius;

		if(!Lua::luaConv(luaPos, pos) || !Lua::luaConv(luaRadius, radius)){
			LOGE("Error creating region, invalid position or radius value");
			return -1;
		}

		return createRegion("", pos, radius);
	}


	void expose(LuaObject& meta){
		meta.RegisterObjectDirect("getActorsInRegion", (Physics*)0, &Physics::lua_getActorsInRegion);
		meta.RegisterObjectDirect("createRegion", (Physics*)0, &Physics::lua_createRegion);
	}


}; // </Physics>

}; // </wt>

#endif
