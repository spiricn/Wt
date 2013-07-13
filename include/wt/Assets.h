#ifndef WT_ASSETS_H
#define WT_ASSETS_H


#include "wt/stdafx.h"

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
#include "wt/ZipFileSystem.h"

namespace wt{

class Assets : public AResourceSystem, public Singleton<Assets>{
private:
	Profiler mProfiler;

	ImageManager* mImageManager;
	TextureManager* mTextureManager;
	ModelManager* mModelManager;
	SkyBoxManager* mSkyBoxManager;
	FontManager* mFontManager;
	AnimationManager* mAnimationManager;
	AResourceManager<ASoundBuffer>* mSoundManager;
	AFileSystem* mFileSystem;

	template<class T>
	void serialize(AResourceManager<T>* manager, const String& name, Lua::LuaObject& table){

		fixAbsolutePaths(manager);

		Lua::LuaObject managerTable;
		LUA_NEW_TABLE(managerTable);
		table.Set(name.c_str(), managerTable);

		Lua::LuaObject root;
		LUA_NEW_TABLE(root);
		managerTable.Set("$ROOT", root);

		manager->serialize(root);
	}


	template<class T>
	void deserialize(AResourceManager<T>* manager, const String& name, const Lua::LuaObject& table){
		WT_ASSERT(table.IsTable(),
			"Error deserializing manager - invalid manager table");

		Lua::LuaObject mngrTable = table.Get(name.c_str());

		WT_ASSERT(mngrTable.IsTable(),
			"Error deserializing manager - invalid manager sub-table \"%s\"", name.c_str());

		manager->deserialize(mngrTable.Get("$ROOT"));
	}

	void deserialize(const LuaObject& assets){
		deserialize(mImageManager, "IMAGE_MANAGER", assets);
		deserialize(mTextureManager, "TEXTURE_MANAGER", assets);
		deserialize(mSkyBoxManager, "SKYBOX_MANAGER", assets);
		deserialize(mAnimationManager, "ANIMATION_MANAGER", assets);
		deserialize(mModelManager, "MODEL_MANAGER", assets);
		deserialize(mSoundManager, "SOUND_MANAGER", assets);
	}

public:
	enum FileSystemType{
		eFS_DIR,
		eFS_ZIP,
	}; //</FileSystemType>

	Assets(FileSystemType fileSystemType, const String& fileSystemRoot){
		if(fileSystemType == eFS_DIR){
			mFileSystem = new LocalFileSystem(fileSystemRoot);
		}
		else{
			mFileSystem = new ZipFileSystem(fileSystemRoot);
		}

		mImageManager = new ImageManager(this);
		mTextureManager = new TextureManager(this);
		mAnimationManager = new AnimationManager(this);
		mModelManager = new ModelManager(this);
		mSkyBoxManager = new SkyBoxManager(this);
		mSoundManager = new SFSoundManager(this);

		// TODO remvoe singleton
		mFontManager = &FontManager::getSingleton();

		mImageManager->setLoader( &DevilImageLoader::getSingleton() );
		mTextureManager->setLoader( &TextureLoader::getSingleton() );
		mSkyBoxManager->setLoader( &SkyBoxLoader::getSingleton() );
		mModelManager->setLoader( &ModelLoader::getSingleton() );
		mAnimationManager->setLoader( &AnimationLoader::getSingleton() );
		mSoundManager->setLoader( &SFSoundLoader::getSingleton() );
	}

	AFileSystem* getFileSystem(){
		return mFileSystem;
	}

	ImageManager* getImageManager(){
		return mImageManager;
	}

	TextureManager* getTextureManager(){
		return mTextureManager;
	}

	FontManager* getFontManager(){
		return mFontManager;
	}

	ModelManager* getModelManager(){
		return mModelManager;
	}

	SkyBoxManager* getSkyBoxManager(){
		return mSkyBoxManager;
	}

	AnimationManager* getAnimationManager(){
		return mAnimationManager;
	}

	AResourceManager<ASoundBuffer>* getSoundManager(){
		return mSoundManager;
	}

	template<class T>
	void fixAbsolutePaths(AResourceManager<T>* manager){
		String homeDir = Utils::getHomeDir();

		for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){

			i->setUri(
				Utils::toRelative(homeDir, i->getUri())
				);
		}
	}

	template<class T>
	void load(AResourceManager<T>* manager){
		for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
			T* r = *i;
			manager->getLoader()->load(r->getUri(), r);
			mLoadedResources++;
			mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
		}
	}

	template<class T>
	void create(AResourceManager<T>* manager){
		for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
			T* r = *i;
			manager->getLoader()->create(r);
			mLoadedResources++;

			mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
		}
	}

	void unloadAll(){
		LOGD("Unloading assets..");
		mImageManager->destroy();
		mTextureManager->destroy();
		mSkyBoxManager->destroy();
		mAnimationManager->destroy();
		mModelManager->destroy();
		mSoundManager->destroy();
		mFontManager->destroy();

		LOGD("Done..");
	}

	void reload(){
		LOGD("Loading images ...");
		mImageManager->loadAll();

		LOGD("Loading textures ...");
		mTextureManager->loadAll();

		LOGD("Loading skyboxes ...");
		mSkyBoxManager->loadAll();

		LOGD("Loading animations ...");
		mAnimationManager->loadAll();

		LOGD("Loading models ...");
		mModelManager->loadAll();

		LOGD("Loading sounds...");
		mSoundManager->loadAll();

		mImageManager->createAll();
		mTextureManager->createAll();
		mSkyBoxManager->createAll();
		mAnimationManager->createAll();
		mSoundManager->createAll();
		mModelManager->createAll();
	}


	void load(const LuaObject& table){
		deserialize(table);

		reload();
	}

	void load(const String& path){
		LuaPlus::LuaStateOwner state;
		if(state->DoFile(path.c_str())){
			WT_THROW("Error executing asset script");
		}

		LuaObject assets = state->GetGlobal("ASSETS");

		load(assets);
	}


	void serialize(LuaObject& assets){
		serialize(mImageManager, "IMAGE_MANAGER", 
			assets);
		serialize(mTextureManager, "TEXTURE_MANAGER", 
			assets);
		serialize(mSkyBoxManager, "SKYBOX_MANAGER", 
			assets);
		serialize(mModelManager, "MODEL_MANAGER", 
			assets);
		serialize(mAnimationManager, "ANIMATION_MANAGER", 
			assets);
		serialize(mSoundManager, "SOUND_MANAGER", 
			assets);
	}

	void serialize(const String& path){
		Lua::LuaObject assets;
		LUA_NEW_TABLE(assets);

		serialize(assets);

		std::ofstream file(path.c_str());

		file << "ASSETS=";
		Lua::serializeTable(assets, file);


		file.close();
	}

	~Assets(){
		delete mImageManager;
		delete mTextureManager;
		delete mModelManager;
		delete mSkyBoxManager;
		delete mAnimationManager;
		delete mSoundManager;
		delete mFileSystem;
	}

}; // </Assets>

}; // </wt>

#endif
