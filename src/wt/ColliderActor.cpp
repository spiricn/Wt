#include "wt/stdafx.h"
#include "wt/ColliderActor.h"
#include "wt/PhysicsActor.h"

#define TD_TRACE_TAG "ColliderActor"

namespace wt
{

ColliderActor::ColliderActor(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_COLLIDER, id, name){
}

ColliderActor::~ColliderActor(){
}
	
ATransformable* ColliderActor::getTransformable() const{
	if(getPhysicsActor()){
		return getPhysicsActor();
	}
	else{
		return &const_cast<ColliderActor*>(this)->mTransform;
	}
}

void ColliderActor::setTranslation(const glm::vec3& translation){
}

void ColliderActor::setRotation(const glm::quat& rotation){
}

void ColliderActor::setScale(const glm::vec3& scale){
}

void ColliderActor::getScale(glm::vec3& result) const{
}

void ColliderActor::getTranslation(glm::vec3& result) const{
}

void ColliderActor::getRotation(glm::quat& result) const{
}

void ColliderActor::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst, opaque);

	// TODO duplicate code from ModelledActor
	if(getPhysicsActor()){
		lua::LuaObject physicsActor = assets->getLuastate()->newTable();

		dst.Set("physics", physicsActor);

		const_cast<PhysicsActor::Desc&>(getPhysicsActor()->getDesc()).serialize(physicsActor);
	}
	else{
		dst.Set("physics", 0);
	}		
}

void ColliderActor::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	DeserializationData* deserData = static_cast<DeserializationData*>(opaque);

	ASceneActor::deserialize(assets, src, opaque);

	// TODO duplicate code from ModelledActor
	if(deserData){
		const LuaObject& luaDesc = src.Get("physics");
		if(luaDesc.IsTable()){

			math::Transform tf;
			glm::vec3 pos;
			glm::quat rot;

			mTransform.getTranslation(pos);
			mTransform.getRotation(rot);

			tf.setTranslation(pos);
			tf.setRotation(rot);

			tf.getMatrix(deserData->pxDesc.pose);

			deserData->phyiscs = true;
			deserData->pxDesc.deserialize(luaDesc);
		}
		else{
			deserData->phyiscs = false;
		}
	}
}

void ColliderActor::physicsControl(const glm::vec3&){
}

void ColliderActor::physicsControl(const glm::quat&){
}

} // </wt>

