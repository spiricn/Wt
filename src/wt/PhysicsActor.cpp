#include "wt/stdafx.h"

#include "wt/PhysicsActor.h"
#include "wt/PxWt.h"
#include "wt/ASceneActor.h"

#define TD_TRACE_TAG "PhysicsActor"

using namespace physx;

namespace wt
{

PhysicsActor::PhysicsActor(uint32_t id, const String& name, ActorType type, ControlMode mode, PxActor* actor,
	PxController* controller, ASceneActor* sceneActor) : mActorType(type), mControlMode(mode),
	mPxActor(actor), mPxController(controller), mSceneActor(sceneActor), mName(name), mId(id){
}

PhysicsActor::Desc::Desc() : type(eTYPE_INVALID), geometryType(eGEOMETRY_INVALID), controlMode(eCTRL_MODE_NONE), collisionMask(0xFFFFFFFF), group(0x01){
}

void PhysicsActor::setTranslation(const glm::vec3& translation){
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
	pxConvert(translation, pose.p);

	pxActor->setGlobalPose(pose, true);
	
	if(pxActor->isRigidStatic() && mSceneActor){
		glm::quat rotation;
		mSceneActor->getTransformable()->getRotation(rotation);

		const_cast<ASceneActor*>(mSceneActor)->physicsControl(translation, rotation);
	}
}

void PhysicsActor::setRotation(const glm::quat& rotation){
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
	pxConvert(rotation, pose.q);

	pxActor->setGlobalPose(pose, true);

	if(pxActor->isRigidStatic() && mSceneActor){
		glm::vec3 translation;
		mSceneActor->getTransformable()->getTranslation(translation);

		const_cast<ASceneActor*>(mSceneActor)->physicsControl(translation, rotation);
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
	table.Set("type", type == eTYPE_STATIC ? "static" : "dynamic");

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