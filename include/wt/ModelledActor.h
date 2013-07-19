#ifndef WT_MODELLEDACTOR_H
#define WT_MODELLEDACTOR_H

#include "wt/ASceneActor.h"
#include "wt/ASerializable.h"
#include "wt/SkeletalAnimationPlayer.h"
#include "wt/FileIOStream.h"
#include "wt/PhysicsActor.h"

namespace wt{


class ModelledActor : public ASceneActor{
private:
	Model* mModel;
	Model::GeometrySkin* mSkin;
	SkeletalAnimationPlayer* mAnimationPlayer;

public:
	struct DeserializationData{
		PhysicsActor::Desc pxDesc;
		bool phyiscs;
	};

	ModelledActor(Scene* parent, uint32_t id, const String& name="") : ASceneActor(parent, ASceneActor::eTYPE_MODELLED, id, name),
		mModel(NULL), mSkin(NULL), mAnimationPlayer(NULL){
	}

	bool hasAnimation() const{
		return mAnimationPlayer != NULL;
	}

	Model* getModel() const{
		return mModel;
	}

	void setModel(Model* model, const String& skin){
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

	

	SkeletalAnimationPlayer* getAnimationPlayer() const{
		return mAnimationPlayer;
	}

	Model::GeometrySkin* getSkin() const{
		return mSkin;
	}

	void setSkin(const String& name){
		setSkin( mModel->getSkin(name) );
	}

	void setSkin(Model::GeometrySkin* skin){
		WT_ASSERT(skin != NULL, "Can't set NULL skin for actor \"%s\"", getName().c_str());

		mSkin = skin;
	}

	bool validAttachPoint(const String& pointId) const{
		return mModel->getSkeleton()->findChildByName(pointId, true) != NULL;
	}

	bool getAttachPointTransform(const String& point, math::Transform& res) const{
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

		res = getTransform() * boneTransf;

		return true;
	}

	///**
	// * Attach self to other actor's bone
	// */
	//void attach(ASceneActor* aActor, const String& bone, float distance=0.0f){
	//	WT_ASSERT(aActor->getActorType() != eTYPE_MODELLED, "Invalid actor type");


	//	ModelledActor* actor = static_cast<ModelledActor*>(aActor);
	//	mAttachDistance = distance;

	//	mAttachActor = actor;
	//	mAttachBone = actor->getModel()->getSkeleton()->findChildByName(bone);

	//	WT_ASSERT(mAttachBone, "No bone named \"%s\"", bone.c_str());
	//}

	void update(float dt){
		if(mAnimationPlayer){
			mAnimationPlayer->advance(dt);
		}

		ASceneActor::update(dt);
	}

	/*********************************/
	/*********** Serialization *******/
	/*********************************/

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL){
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
			Lua::LuaObject physicsActor;
			LUA_NEW_TABLE(physicsActor);

			dst.Set("physics", physicsActor);

			const_cast<PhysicsActor::Desc&>(getPhysicsActor()->getDesc()).serialize(physicsActor);
		}
		else{
			dst.Set("physics", 0);
		}		
	}

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
		DeserializationData* deserData = static_cast<DeserializationData*>(opaque);

		// ASceneActor is going to do the transform deserialization
		ASceneActor::deserialize(assets, src);

		String modelPath, skin;
		
		// Model  & animation
		if(Lua::luaConv(src.Get("model"), modelPath)){
			Lua::luaConv(src.Get("skin"), skin);

			setModel( assets->getModelManager()->getFromPath(modelPath), skin );

			String animation;
			bool loop = false;
			float speed = 1.0f;
			float pos = 0.0f;

			if(Lua::luaConv(src.Get("ani"), animation)){
				Lua::luaConv(src.Get("ani.loop"), loop);
				Lua::luaConv(src.Get("ani.pos"), pos);
				Lua::luaConv(src.Get("ani.speed"), speed);

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
}; // </ModeledActor>

}; // </wt>

#endif // </WT_MODELLEDACTOR_H>
