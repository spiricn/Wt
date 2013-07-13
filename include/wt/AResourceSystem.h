#pragma once
#ifndef WT_ARESOURCESYSTEM_H
#define WT_ARESOURCESYSTEM_H

#include "wt/AFileSystem.h"
#include "wt/ASound.h"

namespace wt{

class ImageManager;
class TextureManager;
class FontManager;
class ModelManager;
class SkyBoxManager;
class AnimationManager;

template<class T>
class AResourceManager;

class AResourceSystem{
public:
	virtual ~AResourceSystem(){
	}

	virtual AFileSystem* getFileSystem() = 0;

	virtual ImageManager* getImageManager() = 0;

	virtual TextureManager* getTextureManager() = 0;

	virtual FontManager* getFontManager() = 0;

	virtual ModelManager* getModelManager() = 0;

	virtual SkyBoxManager* getSkyBoxManager() = 0;

	virtual AnimationManager* getAnimationManager() = 0;

	virtual AResourceManager<ASoundBuffer>* getSoundManager() = 0;
};

}; // </wt>

#endif // </WT_ARESOURCESYSTEM_H>