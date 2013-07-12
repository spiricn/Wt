#ifndef WT_ACTOR_H
#define WT_ACTOR_H


#include "wt/stdafx.h"


#include "wt/Transform.h"
#include "wt/Model.h"
#include "wt/ASerializable.h"
#include "wt/SkeletalAnimationPlayer.h"

namespace wt{

class PhysicsActor;



class SceneActor{
protected:
	math::Transform mTransform;
	PhysicsActor* mPhysicsActor;
	String mName;
	Model* mModel;
	Model::GeometrySkin* mSkin;
	SkeletalAnimationPlayer* mAnimationPlayer;
	SceneActor* mAttachActor;
	SkeletonBone* mAttachBone;
	float mAttachDistance;
	bool mIsRayCaster;
	void* mUserData;
	Uint32 mId;
	bool mUserDataSet;

public:
	SceneActor(Uint32 id, const String& name="") : mName(name), mId(id),
		mModel(NULL), mPhysicsActor(NULL), mAttachBone(NULL), mUserData(NULL),
		mAttachActor(NULL), mAttachDistance(0.0f), mIsRayCaster(true), mAnimationPlayer(NULL), mUserDataSet(false){
	}

	void* getUserData() const{
		return mUserData;
	}

	void setUserData(void* data){
		mUserDataSet = true;
		mUserData = data;
	}

	bool hasUserData() const{
		return mUserDataSet;
	}

	virtual ~SceneActor(){
	}

	Uint32 getId() const{
		return mId;
	}

	bool isRayCaster() const{
		return mIsRayCaster;
	}

	bool hasAnimation() const{
		return mAnimationPlayer != NULL;
	}

	Model* getModel(){
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

	SkeletalAnimationPlayer* getAnimationPlayer(){
		return mAnimationPlayer;
	}

	Model::GeometrySkin* getSkin(){
		return mSkin;
	}

	void setSkin(const String& name){
		setSkin( mModel->getSkin(name) );
	}

	void setSkin(Model::GeometrySkin* skin){
		WT_ASSERT(skin != NULL, "Can't set NULL skin for actor \"%s\"", getName().c_str());

		mSkin = skin;
	}

	// attach self to actors bone
	void attach(SceneActor* actor, const String& bone, float distance=0.0f){
		mAttachDistance = distance;

		mAttachActor = actor;
		mAttachBone = actor->getModel()->getSkeleton()->findChildByName(bone);

		WT_ASSERT(mAttachBone, "No bone named \"%s\"", bone.c_str());
	}

	virtual physx::PxBounds3 getBounds(){
	/*	if(mPhysicsActor != NULL){
			return  mPxActor->getWorldBounds();
		}
		else{
			*/
			return physx::PxBounds3();
		//}
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

	const String& getName() const{
		return mName;
	}

	math::Transform& getTransform(){
		return mTransform;
	}

	PhysicsActor* getPhysicsActor(){
		return mPhysicsActor;
	}

	void setPhysicsActor(PhysicsActor* actor){
		mPhysicsActor = actor;
	}

}; // </SceneActor>

}; // </wt>

#endif
