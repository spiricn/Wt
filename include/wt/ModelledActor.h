#ifndef WT_MODELLEDACTOR_H
#define WT_MODELLEDACTOR_H

#include "wt/ASceneActor.h"

namespace wt{


class ModelledActor : public ASceneActor{
private:
	Model* mModel;
	Model::GeometrySkin* mSkin;
	SkeletalAnimationPlayer* mAnimationPlayer;
	ModelledActor* mAttachActor;
	SkeletonBone* mAttachBone;
	float mAttachDistance;

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
}; // </ModeledActor>

}; // </wt>

#endif // </WT_MODELLEDACTOR_H>