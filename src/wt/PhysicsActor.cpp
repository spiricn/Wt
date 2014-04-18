#include "wt/stdafx.h"

#include "wt/PhysicsActor.h"
#include "wt/PxWt.h"
#include "wt/ASceneActor.h"

// TODO handle circular dependency
#include "wt/Physics.h"

#define TD_TRACE_TAG "PhysicsActor"

using namespace physx;

namespace wt
{

PhysicsActor::PhysicsActor(Physics* parent, uint32_t id, const String& name, ActorType type, ControlMode mode, PxActor* actor,
	PxController* controller, ASceneActor* sceneActor) : mActorType(type), mControlMode(mode),
	mPxActor(actor), mPxController(controller), mSceneActor(sceneActor), mName(name), mId(id), mSceneActorCtrl(true), mParent(parent){
}

physx::PxU32 PhysicsActor::move(const glm::vec3& disp, float dt, float minDistance){
	if(!isControlled()){
		TRACEW("Attempting to move non-CCT actor");
		return 0;
	}

	physx::PxVec3 pxDisp;
	pxConvert(disp, pxDisp);

	PxControllerFilters filters;
	filters.mFilterCallback = mParent;

	// Get the actor shape
	physx::PxShape* shapes[1];
	uint32_t numShapes = getController()->getActor()->getShapes(&shapes[0], 1, 0);
	
	// Set the filter data
	physx::PxFilterData filterData = shapes[0]->getSimulationFilterData();
	filters.mFilterData = &filterData;

	// Once we assembled the disposition vector we can move the actor
	return getController()->move(pxDisp, minDistance, dt, filters);
}

PhysicsActor::Desc::Desc() : type(eTYPE_INVALID), geometryType(eGEOMETRY_INVALID), controlMode(eCTRL_MODE_NONE), collisionMask(0xFFFFFFFF), group(0x01), sceneActorCtrl(true){
}


void PhysicsActor::setSceneActorCtrl(bool state){
	mSceneActorCtrl = state;
}

bool PhysicsActor::getSceneActorCtrl() const{
	return mSceneActorCtrl;
}

void PhysicsActor::setTranslation(const glm::vec3& translation){
	PxController* ctrl = getController();

	if(ctrl){
		PxExtendedVec3 pos;
		pxConvert(translation, pos);

		ctrl->setFootPosition(pos);
	}
	else{
		PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

		PxTransform pose = pxActor->getGlobalPose();
		pxConvert(translation, pose.p);

		pxActor->setGlobalPose(pose, true);
	
		if(pxActor->isRigidStatic() && mSceneActor){
			const_cast<ASceneActor*>(mSceneActor)->physicsControl(translation);
		}
	}
}

void PhysicsActor::setRotation(const glm::quat& rotation){
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
	pxConvert(rotation, pose.q);

	pxActor->setGlobalPose(pose, true);

	if(pxActor->isRigidStatic() && mSceneActor){
		const_cast<ASceneActor*>(mSceneActor)->physicsControl(rotation);
	}
}

void PhysicsActor::setScale(const glm::vec3& scale){
}

void PhysicsActor::getScale(glm::vec3& result) const{
}

void PhysicsActor::getTranslation(glm::vec3& result) const{
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
		
	pxConvert(pose.p, result);
}

void PhysicsActor::getRotation(glm::quat& result) const{
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
		
	pxConvert(pose.q, result);
}

void PhysicsActor::Desc::serialize(LuaPlus::LuaObject& table){
	// Actor type
	if(type == eTYPE_STATIC){
		table.Set("type", "static");
	}
	else if(type == eTYPE_DYNAMIC){
		table.Set("type", "dynamic");
	}
	else if(type == eTYPE_REGION){
		table.Set("type", "region");
	}
	else{
		WT_THROW("Not implemented");
	}

	// Controll
	table.Set("group", group);
	int32_t collision = collisionMask;
	table.Set("collision_mask", collision);


	// Physics-controlled
	if(controlMode == eCTRL_MODE_PHYSICS){
		table.Set("ctrl.mode", "physics");

		// Box
		if(geometryType == eGEOMETRY_BOX){
			table.Set("geo.shape", "box");
			table.Set("geo.hx", geometryDesc.boxGeometry.hx);
			table.Set("geo.hy", geometryDesc.boxGeometry.hy);
			table.Set("geo.hz", geometryDesc.boxGeometry.hz);
		}
		// Sphere
		else if(geometryType == eGEOMETRY_SPHERE){
			table.Set("geo.shape", "sphere");
			table.Set("geo.rad", geometryDesc.sphereGeometry.radius);
		}
		else if(geometryType == eGEOMETRY_HEIGHTMAP){
			table.Set("geo.shape", "heightmap");
		}
		else if(geometryType == eGEOMETRY_MESH){
			table.Set("geo.shape", "mesh");
		}
		else if(geometryType == eGEOMETRY_PLANE){
			table.Set("geo.shape", "plane");
		}
		else{
			WT_THROW("TODO");
		}
	}
	// Controller-controlled
	else if(controlMode == eCTRL_MODE_CONTROLLER){
		table.Set("ctrl.mode", "controller");

		if(controllerDesc.geometryType == eCTRL_GEOMETRY_CAPSULE){
			table.Set("ctrl.shape", "capsule");
			table.Set("ctrl.rad", controllerDesc.geometryDesc.capsuleController.radius);
			table.Set("ctrl.height", controllerDesc.geometryDesc.capsuleController.height);
		}
		else if(controllerDesc.geometryType == eCTRL_GEOMETRY_BOX){
			table.Set("ctrl.shape", "box");
			table.Set("ctrl.hx", controllerDesc.geometryDesc.boxController.hx);
			table.Set("ctrl.hy", controllerDesc.geometryDesc.boxController.hy);
			table.Set("ctrl.hz", controllerDesc.geometryDesc.boxController.hz);
		}
		else{
			WT_THROW("TODO");
		}
	}
	else{
		WT_THROW("TODO");
	}
}


void PhysicsActor::Desc::deserialize(const LuaPlus::LuaObject& table){
	String type, ctrlMode;
	lua::luaConv(table.Get("type"), type);
	lua::luaConv(table.Get("ctrl.mode"), ctrlMode);

	if(!type.compare("static")){
		this->type = eTYPE_STATIC;
	}
	else if(!type.compare("dynamic")){
		this->type = eTYPE_DYNAMIC;
	}
	else if(!type.compare("region")){
		this->type = eTYPE_REGION;
	}
	else{
		WT_THROW("TODO");
	}

	// Controll
	group = table.Get("group").ToInteger();
	collisionMask = table.Get("collision_mask").ToInteger();
	
	//TRACEI("%#x", mask);

	// Physics-controlled
	if(!ctrlMode.compare("physics")){
		controlMode = eCTRL_MODE_PHYSICS;

		String shape;
		lua::luaConv(table.Get("geo.shape"), shape);

		if(!shape.compare("box")){
			this->geometryType = eGEOMETRY_BOX;

			lua::luaConv(table.Get("geo.hx"), geometryDesc.boxGeometry.hx);
			lua::luaConv(table.Get("geo.hy"), geometryDesc.boxGeometry.hy);
			lua::luaConv(table.Get("geo.hz"), geometryDesc.boxGeometry.hz);
		}
		else if(!shape.compare("sphere")){
			this->geometryType = eGEOMETRY_SPHERE;

			lua::luaConv(table.Get("geo.rad"), geometryDesc.sphereGeometry.radius);
		}
		else if(!shape.compare("mesh")){
			this->geometryType = eGEOMETRY_MESH;
		}
		else if(!shape.compare("heightmap")){
			this->geometryType = eGEOMETRY_HEIGHTMAP;
		}
		else if(!shape.compare("plane")){
			this->geometryType = eGEOMETRY_PLANE;
		}
		else{
			WT_THROW("TODO");
		}
	}
	// Controller-controlled
	else if(!ctrlMode.compare("controller")){
		controlMode = eCTRL_MODE_CONTROLLER;

		String shape;
		lua::luaConv(table.Get("ctrl.shape"), shape);

		if(!shape.compare("capsule")){
			this->controllerDesc.geometryType = eCTRL_GEOMETRY_CAPSULE;
			lua::luaConv(table.Get("ctrl.rad"), controllerDesc.geometryDesc.capsuleController.radius);
			lua::luaConv(table.Get("ctrl.height"), controllerDesc.geometryDesc.capsuleController.height);
		}
		else if(!shape.compare("box")){
			this->controllerDesc.geometryType = eCTRL_GEOMETRY_BOX;
			lua::luaConv(table.Get("ctrl.hx"), controllerDesc.geometryDesc.boxController.hx);
			lua::luaConv(table.Get("ctrl.hy"), controllerDesc.geometryDesc.boxController.hy);
			lua::luaConv(table.Get("ctrl.hz"), controllerDesc.geometryDesc.boxController.hz);
		}
		else{
			WT_THROW("TODO");
		}
	}
	else{
		WT_THROW("TODO");
	}
}

bool PhysicsActor::Desc::isValid() const{
	if(geometryType == eGEOMETRY_MESH){
		return geometryDesc.meshGeometry.model != NULL;
	}
	else{
		return true;
	}
}

void PhysicsActor::setController(PxController* ctrl){
	mPxController = ctrl;
}

void PhysicsActor::setDesc(const Desc& desc){
	mDesc = desc;
}

PhysicsActor::ControlMode PhysicsActor::getControlMode() const{
	return mControlMode;
}

bool PhysicsActor::isControlled() const{
	return mControlMode == eCTRL_MODE_CONTROLLER;
}

PxController* PhysicsActor::getController(){
	return mPxController;
}


PxActor* PhysicsActor::getPxActor(){
	return mPxActor;
}

const String& PhysicsActor::getName() const{
	return mName;
}

PhysicsActor::ActorType PhysicsActor::getType() const{
	return mActorType;
}

ASceneActor* PhysicsActor::getSceneActor() const{
	return mSceneActor;
}

uint32_t PhysicsActor::getId() const{
	return mId;
}

const PhysicsActor::Desc& PhysicsActor::getDesc() const{
	return mDesc;
}

} // </wt>