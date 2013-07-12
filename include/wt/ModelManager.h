#ifndef WT_MODELMANAGER_H
#define WT_MODELMANAGER_H


#include "wt/stdafx.h"

#include "wt/Model.h"
#include "wt/Singleton.h"

namespace wt{

class ModelManager : public AResourceManager<Model>{
private:
	TextureManager* mTextureManager;
	AnimationManager* mAnimationManager;

public:
	ModelManager(TextureManager* textureManager, AnimationManager* animationManager) : mTextureManager(textureManager), mAnimationManager(animationManager){
	}

	Model* create(const String& name){
		Model* res = AResourceManager<Model>::create(name);

		res->setTextureManager(mTextureManager);
		res->setAnimationManager(mAnimationManager);

		return res;
	}

}; // </ModelManager>

}; // </wt>

#endif
