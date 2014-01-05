#include "wt/stdafx.h"
#include "wt/ASceneActor.h"
#include "wt/PhysicsActor.h"
#include "wt/Scene.h"
#include "wt/ATransformable.h"

#include "wt/Pb.h"

namespace wt{


ASceneActor::ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name) : mName(name), mId(id),
	mUserData(NULL), mUserDataSet(false), mParent(parent), mPhysicsActor(NULL), mType(type), mBBox(NULL),
	mBoundingBoxColor(Color::Green()), mLuaState(NULL), mVisible(true){
}

void ASceneActor::setId(uint32_t id){
	mId = id;
}

ASceneActor::ActorType ASceneActor::conv(pb::ActorType type){
	switch(type){
	case pb::eTYPE_MODELLED:
		return ASceneActor::eTYPE_MODELLED;
	case pb::eTYPE_PARTICLE:
		return ASceneActor::eTYPE_PARTICLE_EFFECT;
	case pb::eTYPE_POINT_LIGHT:
		return ASceneActor::eTYPE_POINT_LIGHT;
	case pb::eTYPE_SOUND:
		return ASceneActor::eTYPE_SOUND;
	case pb::eTYPE_TERRAIN:
		return ASceneActor::eTYPE_TERRAIN;
	default:
		WT_THROW("Invalid type %d", type);
	}
}

pb::ActorType ASceneActor::conv(ASceneActor::ActorType type){
	switch(type){
	case ASceneActor::eTYPE_MODELLED:
		return pb::eTYPE_MODELLED;
	case ASceneActor::eTYPE_PARTICLE_EFFECT:
		return pb::eTYPE_PARTICLE;
	case ASceneActor::eTYPE_POINT_LIGHT:
		return pb::eTYPE_POINT_LIGHT;
	case ASceneActor::eTYPE_SOUND:
		return pb::eTYPE_SOUND;
	case ASceneActor::eTYPE_TERRAIN:
		return pb::eTYPE_TERRAIN;
	default:
		WT_THROW("Invalid type %d", type);
	}
}

void ASceneActor::serialize(pb::SceneActor* dst) const{
	//Transform
	pb::conv(*getTransformable(), dst->mutable_base()->mutable_transform());

	//Name
	dst->mutable_base()->set_name(mName);

	// Bounding box
	glm::vec3 bbox(1, 1, 1);
	pb::conv(bbox, dst->mutable_base()->mutable_bounding_box());

	// Type
	dst->set_type(conv(mType));
}

void ASceneActor::deserialize(const pb::SceneActor& src){
	pb::conv(src.base().transform(), getTransformable());

	mName = src.base().name();
}

bool ASceneActor::isVisible() const{
	return mVisible;
}

void ASceneActor::setVisible(bool state){
	mVisible = state;
}

void ASceneActor::detach(){
	if(mAttachPoint.actor == NULL){
		// Not attached to any actors
		return;
	}

	ActorList::iterator iter = std::find(mAttachPoint.actor->mAttachedActors.begin(), mAttachPoint.actor->mAttachedActors.end(), this);
	WT_ASSERT(iter != mAttachPoint.actor->mAttachedActors.end(), "Sanity check");

	mAttachPoint.actor->mAttachedActors.erase(iter);

	mAttachPoint.actor = NULL;
	mAttachPoint.pointId = "";
}

Scene* ASceneActor::getScene() const{
	return mParent;
}

void ASceneActor::setBoundingBoxColor(const Color& color){
	mBoundingBoxColor = color;
}

const Color& ASceneActor::getBoundingBoxColor() const{
	return mBoundingBoxColor;
}

void ASceneActor::setName(const String& name){
	mName = name;
}

void ASceneActor::destroy(){
	return mParent->deleteActor(this);
}

void ASceneActor::setLuaState(lua::State* state){
	mLuaState = state;
}

lua::State* ASceneActor::getLuaState(){
	return mLuaState;
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

bool ASceneActor::getAttachPointTransform(const String& pointId, glm::mat4& res) const{
	if(pointId.size() == 0){

		getTransformable()->getTransformMatrix(res);

		return true;
	}
	else{
		return false;
	}
}

bool ASceneActor::validAttachPoint(const String& pointId) const{
	return pointId.size() == 0;
}

bool ASceneActor::attach(ASceneActor* actor, const String& pointId){
	if(!actor->validAttachPoint(pointId)){
		return false;
	}
	else{
		mAttachPoint.actor = actor;
		mAttachPoint.pointId = pointId;
		actor->mAttachedActors.push_back(this);
		return true;
	}
}

void ASceneActor::physicsControl(const glm::vec3& translation, const glm::quat& rotation){
	getTransformable()->setTranslation(translation);
	getTransformable()->setRotation(rotation);
}

void ASceneActor::setUserData(void* data){
	mUserDataSet = true;
	mUserData = data;
}

PhysicsActor* ASceneActor::getPhysicsActor() const{
	return mPhysicsActor;
}

void ASceneActor::setPhysicsActor(PhysicsActor* actor){
	mPhysicsActor = actor;
}
bool ASceneActor::hasUserData() const{
	return mUserDataSet;
}

ASceneActor::~ASceneActor(){
	// Detach from any actors currently attached to
	detach();

	// Since detaching attached actors from us will modifiy mAttachedActor list we need a copy of it for iteration
	ActorList tmp;
	std::copy(mAttachedActors.begin(), mAttachedActors.end(), tmp.begin());

	// Detach all attached actors from us
	for(ActorList::iterator iter=tmp.begin(); iter!=tmp.end(); iter++){
		(*iter)->detach();
	}
}

uint32_t ASceneActor::getId() const{
	return mId;
}	

const String& ASceneActor::getName() const{
	return mName;
}

void ASceneActor::update(float /*dt*/){
	if(mBBox){
		// TODO terribly inefficient
		physx::PxRigidStatic* actor =  static_cast<physx::PxRigidStatic*>(mBBox->getPxActor());

		physx::PxTransform pose = actor->getGlobalPose();

		glm::vec3 pos;
		getTransformable()->getTranslation(pos);

		// TODO handle scaling
		pose.p.x = pos.x;
		pose.p.y = pos.y;
		pose.p.z = pos.z;		

		actor->setGlobalPose(pose);
	}

	if(mAttachPoint.actor){
		glm::mat4 tf;
		mAttachPoint.actor->getAttachPointTransform(mAttachPoint.pointId, tf);

		glm::vec3 pos, scale;
		glm::quat rot;

		math::decomposeMatrix(tf, rot, pos, scale);
		glm::mat4 mat = glm::translate(pos) * glm::mat4_cast(rot);

		getTransformable()->setTransformMatrix(mat);
	}
}

void ASceneActor::setParent(Scene* scene){
	mParent = scene;
}

void ASceneActor::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	WT_ASSERT(dst.IsTable(), "Error serializing, object not a table");

	lua::LuaObject tf = assets->getLuastate()->newTable();

	lua::luaConv(*getTransformable(), tf);
	dst.Set("transform", tf);

	dst.Set("name", mName.c_str());


	if(mType == eTYPE_MODELLED){
		dst.Set("type", "modelled");
	}
	else if(mType == eTYPE_TERRAIN){
		dst.Set("type", "terrain");
	}
	else if(mType == eTYPE_PARTICLE_EFFECT){
		dst.Set("type", "particle");
	}
	else if(mType == eTYPE_POINT_LIGHT){
		dst.Set("type", "pointlight");
	}
	else if(mType == eTYPE_SOUND){
		dst.Set("type", "sound");
	}
	else{
		WT_THROW("Unsupported actor type");
	}

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

	if(!lua::luaConv(src.Get("transform"), *getTransformable())){
		WT_THROW("Error deserializing transform");
	}

	if(!lua::luaConv(src.Get("name"), mName)){
		mName = "";
	}
}

}; // </wt>
