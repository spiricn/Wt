#ifndef WT_MODELLEDACTOR_H
#define WT_MODELLEDACTOR_H

#include "wt/ASceneActor.h"
#include "wt/ASerializable.h"
#include "wt/SkeletalAnimationPlayer.h"

namespace wt{


class ModelledActor : public ASceneActor{
private:
	Model* mModel;
	Model::GeometrySkin* mSkin;
	SkeletalAnimationPlayer* mAnimationPlayer;
	ModelledActor* mAttachActor;
	SkeletonBone* mAttachBone;
	float mAttachDistance;

	struct AttachPoint{
		math::Transform transform;
		SkeletonBone* bone;
		ASceneActor* actor;
	};

public:
	ModelledActor(Scene* parent, uint32_t id, const String& name="") : ASceneActor(parent, ASceneActor::eTYPE_MODELLED, id, name),
		mModel(NULL), mSkin(NULL), mAnimationPlayer(NULL), mAttachActor(NULL), mAttachBone(NULL), mAttachDistance(0.0f){
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

	/**
	 * Attach self to other actor's bone
	 */
	void attach(ASceneActor* aActor, const String& bone, float distance=0.0f){
		WT_ASSERT(aActor->getActorType() != eTYPE_MODELLED, "Invalid actor type");


		ModelledActor* actor = static_cast<ModelledActor*>(aActor);
		mAttachDistance = distance;

		mAttachActor = actor;
		mAttachBone = actor->getModel()->getSkeleton()->findChildByName(bone);

		WT_ASSERT(mAttachBone, "No bone named \"%s\"", bone.c_str());
	}

	void update(float dt){
		if(mAnimationPlayer){
			mAnimationPlayer->advance(dt);
		}
		
		if(mAttachBone){
			glm::mat4 boneTransf;
			if(mAttachActor->mAnimationPlayer && mAttachActor->mAnimationPlayer->getCurrentAnimation()){
				boneTransf = mAttachActor->mAnimationPlayer->getGlobalBoneTransform(mAttachBone->getIndex());
			}
			else{
				mAttachBone->getGlobalTransform(boneTransf);
			}
			mTransform = mAttachActor->getTransform() * boneTransf;

			if(mAttachDistance != 0.0f){
				mTransform.moveForward( mAttachDistance );
			}
		}
	}

	/*********************************/
	/*********** Serialization *******/
	/*********************************/

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst){
		ASceneActor::serialize(assets, dst);

		dst.Set("model",
			mModel ? mModel->getPath().c_str() : NULL
			);

		dst.Set("skin",
			mSkin ? mSkin->getName().c_str() : NULL
			);

		if(mAnimationPlayer && mAnimationPlayer->getCurrentAnimation()){
			dst.Set("animation",
				mAnimationPlayer->getCurrentAnimationName().c_str()
			);

			dst.Set("animationLoop",
				mAnimationPlayer->isLooping()
			);
		}
	}

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src){
		// ASceneActor is going to do the transform deserialization
		ASceneActor::deserialize(assets, src);

		const char* modelPath = src.Get("model").ToString();
		const char* skin = src.Get("skin").ToString();

		setModel( assets->getModelManager()->getFromPath(modelPath), skin );

		String animation;
		bool isLooping = false;
		if(Lua::luaConv(src.Get("animation"), animation)){
			Lua::luaConv(src.Get("animationLoop"), isLooping);

			getAnimationPlayer()->play(animation, isLooping);
		}
	}
}; // </ModeledActor>

}; // </wt>

#endif // </WT_MODELLEDACTOR_H>