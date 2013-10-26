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

PhysicsActor::Desc::Desc() : type(eACTOR_TYPE_NONE), geometryType(eGEO_TYPE_NONE), controlMode(eCTRL_MODE_NONE), collisionMask(0xFFFFFFFF), group(0x01){
}

void PhysicsActor::setTranslation(const glm::vec3& translation){
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
	pxConvert(translation, pose.p);

	pxActor->setGlobalPose(pose, true);
	
	if(pxActor->isRigidStatic() && mSceneActor){
		const_cast<ATransformable*>(mSceneActor->getTransformable())->setTranslation(translation);
	}
}

void PhysicsActor::setRotation(const glm::quat& rotation){
	PxRigidActor* pxActor = static_cast<PxRigidActor*>(mPxActor);

	PxTransform pose = pxActor->getGlobalPose();
	pxConvert(rotation, pose.q);

	pxActor->setGlobalPose(pose, true);

	if(pxActor->isRigidStatic() && mSceneActor){
		const_cast<ATransformable*>(mSceneActor->getTransformable())->setRotation(rotation);
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
	table.Set("type", type == eSTATIC_ACTOR ? "static" : "dynamic");

	// Controll
	table.Set("group", group);
	int32_t collision = collisionMask;
	table.Set("collision_mask", collision);


	// Physics-controlled
	if(controlMode == ePHYSICS_MODE){
		table.Set("ctrl.mode", "physics");

		// Box
		if(geometryType == eBOX_GEOMETRY){
			table.Set("geo.shape", "box");
			table.Set("geo.hx", geometryDesc.boxGeometry.hx);
			table.Set("geo.hy", geometryDesc.boxGeometry.hy);
			table.Set("geo.hz", geometryDesc.boxGeometry.hz);
		}
		// Sphere
		else if(geometryType == eSPHERE_GEOMETRY){
			table.Set("geo.shape", "sphere");
			table.Set("geo.rad", geometryDesc.sphereGeometry.radius);
		}
		else if(geometryType == eHEIGHTMAP_GEOMETRY){
			table.Set("geo.shape", "heightmap");
		}
		else if(geometryType == eMESH_GEOMETRY){
			table.Set("geo.shape", "mesh");
		}
		else{
			WT_THROW("TODO");
		}
	}
	// Controller-controlled
	else if(controlMode == eCONTROLLER_MODE){
		table.Set("ctrl.mode", "controller");

		if(controllerDesc.geometryType == eCAPSULE_CONTROLLER){
			table.Set("ctrl.shape", "capsule");
			table.Set("ctrl.rad", controllerDesc.geometryDesc.capsuleController.radius);
			table.Set("ctrl.height", controllerDesc.geometryDesc.capsuleController.height);
		}
		else if(controllerDesc.geometryType == eBOX_CONTROLLER){
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
		this->type = eSTATIC_ACTOR;
	}
	else if(!type.compare("dynamic")){
		this->type = eDYNAMIC_ACTOR;
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
		controlMode = ePHYSICS_MODE;

		String shape;
		lua::luaConv(table.Get("geo.shape"), shape);

		if(!shape.compare("box")){
			this->geometryType = eBOX_GEOMETRY;

			lua::luaConv(table.Get("geo.hx"), geometryDesc.boxGeometry.hx);
			lua::luaConv(table.Get("geo.hy"), geometryDesc.boxGeometry.hy);
			lua::luaConv(table.Get("geo.hz"), geometryDesc.boxGeometry.hz);
		}
		else if(!shape.compare("sphere")){
			this->geometryType = eSPHERE_GEOMETRY;

			lua::luaConv(table.Get("geo.rad"), geometryDesc.sphereGeometry.radius);
		}
		else if(!shape.compare("mesh")){
			this->geometryType = eMESH_GEOMETRY;
		}
		else if(!shape.compare("heightmap")){
			this->geometryType = eHEIGHTMAP_GEOMETRY;
		}
		else{
			WT_THROW("TODO");
		}
	}
	// Controller-controlled
	else if(!ctrlMode.compare("controller")){
		controlMode = eCONTROLLER_MODE;

		String shape;
		lua::luaConv(table.Get("ctrl.shape"), shape);

		if(!shape.compare("capsule")){
			this->controllerDesc.geometryType = eCAPSULE_CONTROLLER;
			lua::luaConv(table.Get("ctrl.rad"), controllerDesc.geometryDesc.capsuleController.radius);
			lua::luaConv(table.Get("ctrl.height"), controllerDesc.geometryDesc.capsuleController.height);
		}
		else if(!shape.compare("box")){
			this->controllerDesc.geometryType = eBOX_CONTROLLER;
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
	if(geometryType == eMESH_GEOMETRY){
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
	return mControlMode == eCONTROLLER_MODE;
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