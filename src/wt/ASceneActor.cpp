#include "wt/stdafx.h"
#include "wt/ASceneActor.h"
#include "wt/PhysicsActor.h"

namespace wt{


ASceneActor::ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name) : mName(name), mId(id),
	mUserData(NULL), mUserDataSet(false), mParent(parent), mPhysicsActor(NULL), mType(type), mBBox(NULL), mBoundingBoxColor(Color::Green()), mLuaState(NULL){
}

void ASceneActor::setBoundingBoxColor(const Color& color){
	mBoundingBoxColor = color;
}

const Color& ASceneActor::getBoundingBoxColor() const{
	return mBoundingBoxColor;
}

void ASceneActor::setBBox(PhysicsActor* box){
	mBBox = box;
	PxShape* shape;
}

void ASceneActor::setBounds(const glm::vec3& bounds){
	if(mBBox){
		PxRigidActor* actor = (PxRigidActor*)mBBox->getPxActor();
		PxTransform tf = actor->getGlobalPose();
	}
}

physx::PxBounds3 ASceneActor::getBounds() const{
	if(mBBox){
		return mBBox->getPxActor()->getWorldBounds();
	}
	else{
		return physx::PxBounds3();
	}
}

const ASceneActor::AttachPoint& ASceneActor::getAttachPoint() const{
	return mAttachPoint;
}

ASceneActor::ActorType ASceneActor::getActorType() const{
	return mType;
}

void* ASceneActor::getUserData() const{
	return mUserData;
}

bool ASceneActor::getAttachPointTransform(const String& pointId, math::Transform& res) const{
	if(pointId.size() == 0){
		res = getTransform();

		return true;
	}
	else{
		return false;
	}
}

bool ASceneActor::validAttachPoint(const String& pointId) const{
	return pointId.size() == 0;
}

bool ASceneActor::attach(const ASceneActor* actor, const String& pointId){
	if(!actor->validAttachPoint(pointId)){
		return false;
	}
	else{
		mAttachPoint.actor = actor;
		mAttachPoint.pointId = pointId;
		return true;
	}
}

void ASceneActor::setUserData(void* data){
	mUserDataSet = true;
	mUserData = data;
}

PhysicsActor* ASceneActor::getPhysicsActor(){
	return mPhysicsActor;
}

void ASceneActor::setPhysicsActor(PhysicsActor* actor){
	mPhysicsActor = actor;
}
bool ASceneActor::hasUserData() const{
	return mUserDataSet;
}

ASceneActor::~ASceneActor(){
}

uint32_t ASceneActor::getId() const{
	return mId;
}	

const String& ASceneActor::getName() const{
	return mName;
}

math::Transform& ASceneActor::getTransform(){
	return mTransform;
}

const math::Transform& ASceneActor::getTransform() const{
	return mTransform;
}

void ASceneActor::update(float /*dt*/){
	if(mBBox){
		// TODO terribly inefficient
		physx::PxRigidStatic* actor =  static_cast<physx::PxRigidStatic*>(mBBox->getPxActor());

		physx::PxTransform pose = actor->getGlobalPose();

		// TODO handle scaling
		pose.p.x = mTransform.getPosition().x;
		pose.p.y = mTransform.getPosition().y;
		pose.p.z = mTransform.getPosition().z;		

		actor->setGlobalPose(pose);
	}

	if(mAttachPoint.actor){
		mAttachPoint.actor->getAttachPointTransform(mAttachPoint.pointId, mTransform);
	}
}

void ASceneActor::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	WT_ASSERT(dst.IsTable(), "Error serializing, object not a table");

	lua::LuaObject tf = assets->getLuastate()->newTable();

	lua::luaConv(mTransform, tf);
	dst.Set("transform", tf);

	dst.Set("name", mName.c_str());

	dst.Set("type",
		mType == eTYPE_MODELLED ? "modelled" : mType == eTYPE_TERRAIN ? "terrain" : "particle"
	);

	if(mBBox){
		lua::LuaObject luaBbox = assets->getLuastate()->newTable();

		luaBbox.Set("hx", 1.0);
		luaBbox.Set("hy", 1.0);
		luaBbox.Set("hz", 1.0);

		dst.Set("bbox", luaBbox);
	}
	else{
		dst.Set("bbox", 0);
	}
}

void ASceneActor::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	WT_ASSERT(src.IsTable(), "Error deserializing, object not a table");

	if(!lua::luaConv(src.Get("transform"), mTransform)){
		WT_THROW("Error deserializing transform");
	}

	if(!lua::luaConv(src.Get("name"), mName)){
		mName = "";
	}
}

}; // </wt>
