#ifndef WT_ANIMATION_H
#define WT_ANIMATION_H


#include "wt/stdafx.h"


#include "wt/Buffer.h"
#include "wt/MatrixStack.h"
#include "wt/NodeAnimation.h"
#include "wt/AResourceManager.h"

/*
Animated models are basically just your normal models, but with added bone and animation information. The meshes for animated models are defined with the vertices in the bindpose positions. That means if you were to render your model with Matrix.Identity for all bone matrices, you'll get the model as you left it in the 1st frame of the animation (the bind pose) in your modelling application. This is normally the typical 'T-Pose', with the characters arms outstretched, level with the shoulders. The transformation matrices in the bindPose[] array match the bone orientations in this bindpose. So essentially the vertices and the bindPose[] matrices describe your base model without any animations.

Moving on, bones are effectively just transformation matrices, which are used in a hierarchy. This means that each transformation matrix (those not explicitly called absolute) only contains the transformations relative to the parent bone, which in turn contains transformations relative to its parent, all the way up to the root bone with index 0. This is why in AnimationPlayer.UpdateWorldTransforms, each world transform matrix is multiplied with its parent bone's world transform matrix. Both the bindPose[] matrices and the boneTransforms[] matrices retrieved from the animation clip are relative like this and AnimationPlayer.UpdateWorldTransforms converts them to absolute transformation matrices.

These absolute transformations can then be used to transform the vertices depending on their assigned bone weights, but there's a little problem. The vertices are already in the bindpose positions, so we need a way to 'reset' these positions so we can apply our absolute transformations from the animation. This is where the inverseBindPose[] comes in. For each bone, we have to first transform the vertices with the absolute inverse bindpose transform and then with our absolute transformation. These two transformations are packed into one single matrix by AnimationPlayer.UpdateSkinTransforms for each bone, which can then be passed into your shader.

To get back to your question, which is the inverseBindPose[] computed off the absolute transform? Since the bindpose of the model is static, we can precalculate the absolute inverseBindPose for each bone by inverting the bone.AbsoluteTransform and use that whenever we call UpdateSkinTransforms to create our skinning matrices. We could also have constructed this AbsoluteTransform by multiplying the relative bindPose[] matrices for each bone in the hierarchy, but that'd just be a waste of time since we only ever need the absolute inverse bindpose matrices.

Now, why does bindPose[] contain relative transformations then? If you look at AnimationPlayer.StartClip, you'll see that the bindPose[] transformations are copied to boneTransforms[] and that these are only replaced in AnimationPlayer.UpdateBoneTransforms when a transform is currently available from the animation for a specific bone. So basically the bindPose[] transforms double as the default transforms the AnimationPlayer uses until the animation starts playing. Since these boneTransforms[] from the animation are relative, the bindPose[] transforms need to be relative too to fill in here.
*/
namespace wt
{

class Animation : public AResource<Animation> {
public:
	typedef std::vector<NodeAnimation*> NodeAnimationList;

	Animation(AResourceManager<Animation>* manager=NULL, ResourceHandle handle=0, const String& name="");

	void setDuration(float dur);

	float getDuration() const;

	NodeAnimation* addNodeAnimation(const String& name="");

	NodeAnimationList& getNodeAnimationList();

	NodeAnimation* findNodeAnimation(const String& name);

	virtual ~Animation();

private:
	float mDuration;
	NodeAnimationList mNodeAnimations;

}; // </Animation>

class AnimationManager : public AResourceManager<Animation>{
public:
	AnimationManager(AResourceSystem* assets) : AResourceManager(assets){
	}

	virtual ~AnimationManager(){
	}

}; // </AnimationManager>

}; // </wt>

#endif
