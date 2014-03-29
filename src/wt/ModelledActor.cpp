#include "wt/stdafx.h"

#include "wt/ModelledActor.h"

#define TD_TRACE_TAG "ModelledActor"

namespace wt
{

ModelledActor::ModelledActor(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_MODELLED, id, name),
	mModel(NULL), mSkin(NULL){
		mAnimationChannels.first = NULL;
		mAnimationChannels.second = NULL;
}

bool ModelledActor::hasAnimation() const{
	return mAnimationChannels.first != NULL;
}

Model* ModelledActor::getModel() const{
	return mModel;
}

SkeletalAnimationPlayer* ModelledActor::blendToAnimation(const String& name, float time, bool loopBlended){
	mBlendTotal = time;
	mBlendCurrent = 0.0f;

	if(mAnimationChannels.second){
		SkeletalAnimationPlayer* tmp = mAnimationChannels.first;

		mAnimationChannels.first = mAnimationChannels.second;

		delete tmp;

		mAnimationChannels.second = NULL;
	}

	if(mAnimationChannels.first){
		// Add the new channel
		mAnimationChannels.second = new SkeletalAnimationPlayer(mModel);
		mAnimationChannels.second->play(name, loopBlended);
	}
	else{
		// Maybe handle this better ?
		getAnimationPlayer()->play(name, loopBlended);
	}

	return getAnimationPlayer();
}
ATransformable* ModelledActor::getTransformable() const{
	return const_cast<ATransformable*>( dynamic_cast<const ATransformable*>(this) );
}

const Buffer<glm::mat4>& ModelledActor::getBoneMatrices() const{
	return mBoneMatrices;
}

bool ModelledActor::isAnimated() const{
	return ( mAnimationChannels.first && mAnimationChannels.first->isPlaying() ) ||  ( mAnimationChannels.second && mAnimationChannels.second->isPlaying() );
}

void ModelledActor::setModel(Model* model, const String& skin){
	if(mAnimationChannels.first){
		delete mAnimationChannels.first;
		mAnimationChannels.first = NULL;
	}

	if(mAnimationChannels.second){
		delete mAnimationChannels.second;
		mAnimationChannels.second = NULL;
	}

	mBoneMatrices.destroy();

	mModel = model;
	if(!model){
		return;
	}

	if(model->hasSkeleton()){
		mAnimationChannels.first = new SkeletalAnimationPlayer(model);
		mBoneMatrices.create(mAnimationChannels.first->getNumBones());
	}

	setSkin(skin);
}

SkeletalAnimationPlayer* ModelledActor::getAnimationPlayer() const{
	return mAnimationChannels.second ? mAnimationChannels.second : mAnimationChannels.first;
}

ModelSkin* ModelledActor::getSkin() const{
	return mSkin;
}

void ModelledActor::setSkin(const String& name){
	setSkin( mModel->getSkin(name) );
}

void ModelledActor::setSkin(ModelSkin* skin){
	WT_ASSERT(skin != NULL, "Can't set NULL skin for actor \"%s\"", getName().c_str());

	mSkin = skin;
}

bool ModelledActor::validAttachPoint(const String& pointId) const{
	if(mModel->getSkeleton() != NULL && pointId.size()){
		return mModel->getSkeleton()->findChildByName(pointId, true) != NULL;
	}
	else{
		return true;
	}
}

bool ModelledActor::getAttachPointTransform(const String& point, glm::mat4& res) const{
	if(point.size()){
		SkeletonBone* bone = mModel->getSkeleton()->findChildByName(point, true);
		if(!bone){
			return false;
		}

		// TODO handle blending
		glm::mat4 boneTransf;
		if(mAnimationChannels.first && mAnimationChannels.first->getCurrentAnimation()){
			boneTransf = mAnimationChannels.first->getGlobalBoneTransform(bone->getIndex());
		}
		else{
			bone->getGlobalTransform(boneTransf);
		}

		math::Transform tf = mTransform * boneTransf;

		tf.getMatrix(res);

		return true;
	}
	else{
		return ASceneActor::getAttachPointTransform(point, res);
	}
}

void ModelledActor::update(float dt){
	if(getAnimationPlayer()){

		if(mAnimationChannels.second){
			// We're blending
			mBlendCurrent += dt;

			if(mBlendCurrent > mBlendTotal){
				// Finished blending
				SkeletalAnimationPlayer* tmp = mAnimationChannels.first;
				mAnimationChannels.first = mAnimationChannels.second;
				delete tmp;
				mAnimationChannels.second = NULL;
			}
		}

		// Reset bone matrices
		memset(mBoneMatrices.getData(), 0x00, mBoneMatrices.getCapacity()*sizeof(glm::mat4));

		if(mAnimationChannels.first){
			mAnimationChannels.first->advance(dt);
		}

		if(mAnimationChannels.second){
			mAnimationChannels.second->advance(dt);
		}


		for(uint32_t i=0; i<mAnimationChannels.first->getNumBones(); i++){
			if(mAnimationChannels.second){
				// Blending
				const float blendFactor = mBlendCurrent/mBlendTotal;

				mBoneMatrices[i] =
					( (1.0f - blendFactor) * mAnimationChannels.first->getBoneMatrices()[i] ) + 
					( (blendFactor) * mAnimationChannels.second->getBoneMatrices()[i] );
			}
			else{
				mBoneMatrices[i] = mAnimationChannels.first->getBoneMatrices()[i];
			}
		}
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

	if(getAnimationPlayer() && getAnimationPlayer()->getCurrentAnimation()){
		dst.Set("ani",
			getAnimationPlayer()->getCurrentAnimationName().c_str()
		);

		dst.Set("ani.loop",
			getAnimationPlayer()->isLooping()
		);

		dst.Set("ani.pos",
			getAnimationPlayer()->getPosition()
		);

		dst.Set("ani.speed",
			getAnimationPlayer()->getSpeed()
		);

	}

	// TODO move physics serialiaztion to ASceneActor
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


	// TODO move physics related deserializetion to ASceneActor (since every scene actor can have a physics actor)
	if(deserData){
		const LuaObject& luaDesc = src.Get("physics");
		if(luaDesc.IsTable()){
			deserData->phyiscs = true;
			deserData->pxDesc.deserialize(luaDesc);

			math::Transform tf;
			glm::vec3 pos;
			glm::quat rot;

			mTransform.getTranslation(pos);
			mTransform.getRotation(rot);

			tf.setTranslation(pos);
			tf.setRotation(rot);

			tf.getMatrix(deserData->pxDesc.pose);

			if(deserData->pxDesc.geometryType == PhysicsActor::eGEOMETRY_MESH){
				deserData->pxDesc.geometryDesc.meshGeometry.model = mModel;
			}
		}
		else{
			deserData->phyiscs = false;
		}
	}
}

void ModelledActor::physicsControl(const glm::vec3& translation){
	mTransform.setTranslation(translation);
}

void ModelledActor::physicsControl(const glm::quat& rotation){
	mTransform.setRotation(rotation);
}

void ModelledActor::setTranslation(const glm::vec3& translation){
	ATransformable* tf = getPhysicsActor() ? dynamic_cast<ATransformable*>(getPhysicsActor()) : &mTransform;

	tf->setTranslation(translation);
}

void ModelledActor::setRotation(const glm::quat& rotation){
	PhysicsActor* pxActor = getPhysicsActor() ;
	ATransformable* tf = NULL;

	// We don't want to rotate through a controlled actor since all it does is update the position
	if(!pxActor || (pxActor && pxActor->isControlled())){
		tf = &mTransform;
	}
	else{
		tf = dynamic_cast<ATransformable*>(getPhysicsActor());
	}

	tf->setRotation(rotation);
}

void ModelledActor::setScale(const glm::vec3& scale){
	// Scale does not affect the physics actor,
	mTransform.setScale(scale);
}

void ModelledActor::getScale(glm::vec3& scale) const{
	mTransform.getScale(scale);
}

void ModelledActor::getTranslation(glm::vec3& result) const{
	mTransform.getTranslation(result);
}

void ModelledActor::getRotation(glm::quat& result) const{
	mTransform.getRotation(result);
}

}; // </wt>