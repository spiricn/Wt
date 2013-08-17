#include "wt/stdafx.h"

#include "wt/ModelledActor.h"


namespace wt{


ModelledActor::ModelledActor(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_MODELLED, id, name),
	mModel(NULL), mSkin(NULL), mAnimationPlayer(NULL){
}

bool ModelledActor::hasAnimation() const{
	return mAnimationPlayer != NULL;
}

Model* ModelledActor::getModel() const{
	return mModel;
}

ATransformable* ModelledActor::getTransformable(){
	return &mTransform;
}

void ModelledActor::setModel(Model* model, const String& skin){
	if(mAnimationPlayer){
		delete mAnimationPlayer;
		mAnimationPlayer = NULL;
	}

	mModel = model;
	if(!model){
		return;
	}

	if(model->hasSkeleton()){
		mAnimationPlayer = new SkeletalAnimationPlayer(model);
	}

	setSkin(skin);
}

	

SkeletalAnimationPlayer* ModelledActor::getAnimationPlayer() const{
	return mAnimationPlayer;
}

Model::GeometrySkin* ModelledActor::getSkin() const{
	return mSkin;
}

void ModelledActor::setSkin(const String& name){
	setSkin( mModel->getSkin(name) );
}

void ModelledActor::setSkin(Model::GeometrySkin* skin){
	WT_ASSERT(skin != NULL, "Can't set NULL skin for actor \"%s\"", getName().c_str());

	mSkin = skin;
}

bool ModelledActor::validAttachPoint(const String& pointId) const{
	return mModel->getSkeleton()->findChildByName(pointId, true) != NULL;
}

bool ModelledActor::getAttachPointTransform(const String& point, math::Transform& res) const{
	SkeletonBone* bone = mModel->getSkeleton()->findChildByName(point, true);
	if(!bone){
		return false;
	}

	glm::mat4 boneTransf;
	if(mAnimationPlayer && mAnimationPlayer->getCurrentAnimation()){
		boneTransf = mAnimationPlayer->getGlobalBoneTransform(bone->getIndex());
	}
	else{
		bone->getGlobalTransform(boneTransf);
	}

	res = mTransform * boneTransf;

	return true;
}

void ModelledActor::update(float dt){
	if(mAnimationPlayer){
		mAnimationPlayer->advance(dt);
	}

	ASceneActor::update(dt);
}

void ModelledActor::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst);

		
	if(mModel){
		dst.Set("model", mModel->getPath().c_str());
		dst.Set("skin", mSkin->getName().c_str());
	}
	else{
			
		dst.Set("model", 0);
		dst.Set("skin", 0);
	}

	if(mAnimationPlayer && mAnimationPlayer->getCurrentAnimation()){
		dst.Set("ani",
			mAnimationPlayer->getCurrentAnimationName().c_str()
		);

		dst.Set("ani.loop",
			mAnimationPlayer->isLooping()
		);

		dst.Set("ani.pos",
			mAnimationPlayer->getPosition()
		);

		dst.Set("ani.speed",
			mAnimationPlayer->getSpeed()
		);

	}

	// Physics
	if(getPhysicsActor()){
		lua::LuaObject physicsActor = mModel->getManager()->getResourceSystem()->getLuastate()->newTable();

		dst.Set("physics", physicsActor);

		const_cast<PhysicsActor::Desc&>(getPhysicsActor()->getDesc()).serialize(physicsActor);
	}
	else{
		dst.Set("physics", 0);
	}		
}

void ModelledActor::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	DeserializationData* deserData = static_cast<DeserializationData*>(opaque);

	// ASceneActor is going to do the transform deserialization
	ASceneActor::deserialize(assets, src);

	String modelPath, skin;
		
	// Model  & animation
	if(lua::luaConv(src.Get("model"), modelPath)){
		lua::luaConv(src.Get("skin"), skin);

		setModel( assets->getModelManager()->getFromPath(modelPath), skin );

		String animation;
		bool loop = false;
		float speed = 1.0f;
		float pos = 0.0f;

		if(lua::luaConv(src.Get("ani"), animation)){
			lua::luaConv(src.Get("ani.loop"), loop);
			lua::luaConv(src.Get("ani.pos"), pos);
			lua::luaConv(src.Get("ani.speed"), speed);

			getAnimationPlayer()->play(animation, loop);
			getAnimationPlayer()->setSpeed(speed);
			getAnimationPlayer()->setPosition(pos);
		}
	}

	// Physics
	if(deserData){
		const LuaObject& luaDesc = src.Get("physics");
		if(luaDesc.IsTable()){
			deserData->phyiscs = true;
			deserData->pxDesc.deserialize(luaDesc);

			if(deserData->pxDesc.geometryType == PhysicsActor::eMESH_GEOMETRY){
				deserData->pxDesc.geometryDesc.meshGeometry.model = mModel;
			}
		}
		else{
			deserData->phyiscs = false;
		}
	}
}

glm::vec3 ModelledActor::lua_getPosition(){
	return mTransform.getPosition();
}

void ModelledActor::lua_move(const glm::vec3& offset){
	mTransform.translate(offset);
}

void ModelledActor::lua_playAnimation(const char* name, bool loop){
	getAnimationPlayer()->play(name, loop);
}

lua::LuaObject ModelledActor::lua_getTransform(){
	return getLuaState()->box(mTransform);
}

void ModelledActor::generateMetaTable(){
	expose("setSkin", (void (ModelledActor::*)(const String&))(&ModelledActor::setSkin));
	expose("move", &ModelledActor::lua_move);
	expose("getPosition", &ModelledActor::lua_getPosition);
	expose("playAnimation", &ModelledActor::lua_playAnimation);
	expose("getTransform", &ModelledActor::lua_getTransform);
	exposeBase<ASceneActor>("getId", &ASceneActor::getId);
}

}; // </wt>