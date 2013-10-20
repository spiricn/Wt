#ifndef WT_ASSETS_H
#define WT_ASSETS_H


#include "wt/stdafx.h"

#include "wt/FileIOStream.h"
#include "wt/ImageManager.h"
#include "wt/Texture.h"
#include "wt/Material.h"
#include "wt/ModelManager.h"
#include "wt/Model.h"
#include "wt/SkyBox.h"
#include "wt/ModelLoader.h"
#include "wt/AnimationLoader.h"
#include "wt/DevilImageLoader.h"
#include "wt/Timer.h"
#include "wt/AssimpModelLoader.h"
#include "wt/Lua.h"
#include "wt/Font.h"
#include "wt/SFSound.h"
#include "wt/Profiler.h"
#include "wt/Thread.h"
#include "wt/AFileSystem.h"
#include "wt/LocalFileSystem.h"
#include "wt/AResourceSystem.h"
#include "wt/ParticleEffectResource.h"
#include "wt/ZipFileSystem.h"
#include "wt/SFSoundManager.h"
#include "wt/SFSoundLoader.h"

#include "wt/lua/State.h"

namespace wt{

class Assets : public AResourceSystem, public Singleton<Assets>{
public:
	
	Assets(FileSystemType fileSystemType, const String& fileSystemRoot);

	Assets();

	~Assets();

	void setFileSystem(FileSystemType type, const String& rootUri);

	AFileSystem* getFileSystem();

	ImageManager* getImageManager();

	AResourceManager<ParticleEffectResource>* getParticleResourceManager();

	TextureManager* getTextureManager();

	lua::State* getLuastate();

	FontManager* getFontManager();

	ModelManager* getModelManager();

	SkyBoxManager* getSkyBoxManager();

	AnimationManager* getAnimationManager();

	AResourceManager<ASoundBuffer>* getSoundManager();

	template<class T>
	void fixAbsolutePaths(AResourceManager<T>* manager);

	template<class T>
	void load(AResourceManager<T>* manager);

	template<class T>
	void create(AResourceManager<T>* manager);

	ASoundSystem* getSoundSystem();

	void unloadAll();

	void reload();

	String getRelativeURI(const String& uri);

	void load(const LuaObject& table);

	void load(const String& path);

	void serialize(LuaObject& assets);

	void serialize(const String& path);

private:
	void init();

	Profiler mProfiler;

	ImageManager* mImageManager;
	TextureManager* mTextureManager;
	ModelManager* mModelManager;
	SkyBoxManager* mSkyBoxManager;
	FontManager* mFontManager;
	AnimationManager* mAnimationManager;
	SFSoundManager* mSoundManager;
	AFileSystem* mFileSystem;
	ParticleEffectResourceManager* mParticleManager;
	lua::State mLuaState;

	template<class T>
	void serialize(AResourceManager<T>* manager, const String& name, lua::LuaObject& table);

	template<class T>
	void deserialize(AResourceManager<T>* manager, const String& name, const lua::LuaObject& table);

	void deserialize(const LuaObject& assets);

	String mRootDir;

}; // </Assets>

#define TD_TRACE_TAG "Assets"

template<class T>
void Assets::fixAbsolutePaths(AResourceManager<T>* manager){
	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
		i->setUri(
			getRelativeURI(i->getUri())
			);
	}
}

template<class T>
void Assets::load(AResourceManager<T>* manager){
	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
		T* r = *i;
		manager->getLoader()->load(r->getUri(), r);
		mLoadedResources++;
		mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
	}
}

template<class T>
void Assets::create(AResourceManager<T>* manager){
	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
		T* r = *i;
		manager->getLoader()->create(r);
		mLoadedResources++;

		mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
	}
}


template<class T>
void Assets::serialize(AResourceManager<T>* manager, const String& name, lua::LuaObject& table){

	fixAbsolutePaths(manager);

	lua::LuaObject managerTable = getLuastate()->newTable();
	table.Set(name.c_str(), managerTable);

	lua::LuaObject root = getLuastate()->newTable();
	managerTable.Set("$ROOT", root);

	manager->serialize(&mLuaState, root);
}


template<class T>
void Assets::deserialize(AResourceManager<T>* manager, const String& name, const lua::LuaObject& table){
	WT_ASSERT(table.IsTable(),
		"Error deserializing manager - invalid manager table");

	lua::LuaObject mngrTable = table.Get(name.c_str());


	if(!mngrTable.IsTable()){
		LOGW("Manager table \"%s\" not found - skipping deserialization", name.c_str());
		return;
	}

	manager->deserialize(&mLuaState, mngrTable.Get("$ROOT"));
}

#define TD_TRACE_TAG ""

}; // </wt>

#endif
