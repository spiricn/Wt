#include "wt/stdafx.h"
#include "wt/ASceneActor.h"

namespace wt{

ASceneActor::ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name) : mName(name), mId(id),
	mUserData(NULL), mUserDataSet(false), mParent(parent), mPhysicsActor(NULL), mType(type){
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

bool ASceneActor::getAttachPointTransform(const String& /*pointId*/, math::Transform& /*res*/) const{
	return false;
}

bool ASceneActor::validAttachPoint(const String& /*pointId*/) const{
	return false;
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
	if(mAttachPoint.actor){
		mAttachPoint.actor->getAttachPointTransform(mAttachPoint.pointId, mTransform);
	}
}

void ASceneActor::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	WT_ASSERT(dst.IsTable(), "Error serializing, object not a table");

	Lua::LuaObject tf;
	LUA_NEW_TABLE(tf);

	Lua::luaConv(mTransform, tf);
	dst.Set("transform", tf);

	dst.Set("name", mName.c_str());

	dst.Set("type",
			mType == eTYPE_MODELLED ? "modelled" : mType == eTYPE_TERRAIN ? "terrain" : "particle"
		) ;
}

void ASceneActor::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	WT_ASSERT(src.IsTable(), "Error deserializing, object not a table");

	if(!Lua::luaConv(src.Get("transform"), mTransform)){
		WT_THROW("Error deserializing transform");
	}

	if(!Lua::luaConv(src.Get("name"), mName)){
		mName = "";
	}
}

}; // </wt>
