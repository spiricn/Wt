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

SkeletalAnimationPlayer* ModelledActor::blendToAnimation(const String& name, float time, bool loopBlended){
	mBlendTotal = time;
	mBlendCurrent = 0.0f;

	SkeletalAnimationPlayer* res = addAnimationChannel();
	res->play(name, loopBlended);

	return res;
}

ATransformable* ModelledActor::getController(){
	if(getPhysicsActor() != NULL){
		return ASceneActor::getPhysicsActor();
	}
	else{
		return &mTransform;
	}
}

SkeletalAnimationPlayer* ModelledActor::addAnimationChannel(){
	SkeletalAnimationPlayer* res = new SkeletalAnimationPlayer(mModel);

	mAnimationChannels.push_back(res);

	return res;
}

void ModelledActor::removeAnimationChannel(SkeletalAnimationPlayer* chanel){
	mAnimationChannels.erase( std::find(mAnimationChannels.begin(), mAnimationChannels.end(), chanel) );
}

const ATransformable* ModelledActor::getTransformable() const{
	return &mTransform;
}

const Buffer<glm::mat4>& ModelledActor::getBoneMatrices() const{
	return mBoneMatrices;
}

bool ModelledActor::isAnimated() const{
	for(AnimationChannelList::const_iterator iter=mAnimationChannels.begin(); iter!=mAnimationChannels.end(); iter++){
		if((*iter)->isPlaying()){
			return true;
		}
	}

	return false;
}

void ModelledActor::setModel(Model* model, const String& skin){
	if(mAnimationPlayer){
		delete mAnimationPlayer;
		mAnimationPlayer = NULL;
		mBoneMatrices.destroy();
	}

	mModel = model;
	if(!model){
		return;
	}

	if(model->hasSkeleton()){
		mAnimationPlayer = new SkeletalAnimationPlayer(model);
		mAnimationChannels.push_back(mAnimationPlayer);
		mBoneMatrices.create(mAnimationPlayer->getNumBones());
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

		glm::mat4 boneTransf;
		if(mAnimationPlayer && mAnimationPlayer->getCurrentAnimation()){
			boneTransf = mAnimationPlayer->getGlobalBoneTransform(bone->getIndex());
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
	if(mAnimationPlayer){
	/*	mAnimationPlayer->advance(dt);

		memcpy(mBoneMatrices.getData(),
			mAnimationPlayer->getBoneMatrices(),
			mAnimationPlayer->getNumBones() * sizeof(glm::mat4));*/

		if(mAnimationChannels.size() == 2){
			mBlendCurrent += dt;
			if(mBlendCurrent > mBlendTotal){
				mAnimationChannels.erase(mAnimationChannels.begin());
				LOG("done blending");
			}
		}

		// Reset bone matrices
		memset(mBoneMatrices.getData(), 0x00, mBoneMatrices.getCapacity()*sizeof(glm::mat4));


		for(AnimationChannelList::iterator iter=mAnimationChannels.begin(); iter!=mAnimationChannels.end(); iter++){
			(*iter)->advance(dt);
		}


		for(uint32_t i=0; i<mAnimationChannels[0]->getNumBones(); i++){
			if(mAnimationChannels.size() == 2){
				float blendFactor = mBlendCurrent/mBlendTotal;
				//LOG("Blending %f", blendFactor);
				mBoneMatrices[i] =
					( (1.0f - blendFactor) * mAnimationChannels[0]->getBoneMatrices()[i] ) + 
					( (blendFactor) * mAnimationChannels[1]->getBoneMatrices()[i] );
			}
			else{
				mBoneMatrices[i] = mAnimationChannels[0]->getBoneMatrices()[i];
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

			if(deserData->pxDesc.geometryType == PhysicsActor::eGEOMETRY_MESH){
				deserData->pxDesc.geometryDesc.meshGeometry.model = mModel;
			}
		}
		else{
			deserData->phyiscs = false;
		}
	}
}

}; // </wt>