#pragma once
#ifndef WT_ARESOURCESYSTEM_H
#define WT_ARESOURCESYSTEM_H

namespace wt{

// Forward declarations
class Image;
class Animation;
class Font;
class Model;
class SkyBox;
class Animation;
class Texture2D;
class ParticleEffectResource;
class AFileSystem;
class ASoundBuffer;
class ASoundSystem;

namespace lua{
	class State;
}; // </lua>

template<class T>
class AResourceManager;

class AResourceSystem{
public:
	enum FileSystemType{
		eFS_INVALID = -1,

		eFS_DIR = 0,
		eFS_ZIP,
	}; //</FileSystemType>

	virtual ~AResourceSystem(){
	}

	virtual void setFileSystem(FileSystemType type, const String& rootUri) = 0;

	virtual lua::State* getLuastate() = 0;

	virtual AFileSystem* getFileSystem() = 0;

	virtual AResourceManager<Image>* getImageManager() = 0;

	virtual AResourceManager<Texture2D>* getTextureManager() = 0;

	virtual AResourceManager<Font>* getFontManager() = 0;

	virtual AResourceManager<Model>* getModelManager() = 0;

	virtual AResourceManager<SkyBox>* getSkyBoxManager() = 0;

	virtual AResourceManager<Animation>* getAnimationManager() = 0;

	virtual AResourceManager<ASoundBuffer>* getSoundManager() = 0;

	virtual AResourceManager<ParticleEffectResource>* getParticleResourceManager() = 0;

	virtual void unloadAll() = 0;

	virtual void reload() = 0;

	virtual void load(const LuaPlus::LuaObject& table) = 0;

	virtual void load(const String& path) = 0;

	virtual void serialize(LuaPlus::LuaObject& assets) = 0;

	virtual void serialize(const String& path) = 0;

	virtual String getRelativeURI(const String& uri) = 0;

	virtual ASoundSystem* getSoundSystem() = 0;
};

}; // </wt>

#endif // </WT_ARESOURCESYSTEM_H>