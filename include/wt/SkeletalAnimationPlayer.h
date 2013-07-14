#ifndef WT_SKELETALANIMATIONPLAYER_H
#define WT_SKELETALANIMATIONPLAYER_H

#include "wt/SkeletonBone.h"
#include "wt/Buffer.h"
#include "wt/Model.h"
#include "wt/TimeAccumulator.h"

namespace wt{

class SkeletalAnimationPlayer{
private:
	Buffer<glm::mat4> mLocalBoneTransforms; // local bone
	Buffer<glm::mat4> mGlobalTransforms; // global bone (culmulative transforms)
	Buffer<glm::mat4> mGlobalBoneTransforms; // global bone * offset
	Model* mModel;
	SkeletalAnimation* mCurrentAnimation;
	String mCurrentAnimationName;
	float mAnimationTime;
	float mAnimationSpeed;
	TimeAccumulator mTimeAccumulator;
	
	bool mIsLooping;

	void evaluate();

	void calculateGlobalTransforms(SkeletonBone* bone);

	void initTransforms(SkeletonBone* bone);

public:

	const glm::mat4& getGlobalBoneTransform(uint32_t index) const{
		return mGlobalTransforms[index];
	}

	SkeletalAnimation* getCurrentAnimation() const{
		return mCurrentAnimation;
	}

	bool isLooping() const{
		return mIsLooping;
	}

	const String& getCurrentAnimationName() const{
		return mCurrentAnimationName;
	}

	SkeletalAnimationPlayer(Model* model);

	~SkeletalAnimationPlayer();

	void play(const String& name, bool loop);

	void play(SkeletalAnimation* animation, bool loop);

	void advance(float dt);

	void setPosition(float pos);

	bool isPlaying() const;

	void setSpeed(float speed);

	const glm::mat4* getBoneMatrices() const;

	uint32_t getNumBones() const;
}; // </SkeletalAnimationPlayer>

}; // </wt>

#endif // </WT_SKELETALANIMATIONPLAYER_H>