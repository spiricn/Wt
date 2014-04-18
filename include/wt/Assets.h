#ifndef WT_ASSETS_H
#define WT_ASSETS_H


#include "wt/stdafx.h"

#include "wt/FileIOStream.h"
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
#include "wt/Heightmap.h"
#include "wt/lua/State.h"
#include "wt/ScriptResource.h"

namespace wt
{

class Assets : public AResourceSystem, public Singleton<Assets>{
public:
	enum ResourceType{
		eTYPE_INVALID = -1,

		eTYPE_IMAGE,
		eTYPE_TEXTURE,
		eTYPE_SKY_BOX,
		eTYPE_ANIMATION,
		eTYPE_MODEL,
		eTYPE_SOUND,
		eTYPE_PARTICLE,
		eTYPE_HEIGHTMAP,
		eTYPE_SCRIPT,

		eTYPE_MAX
	}; // </ResourceType>

public:
	Assets();

	~Assets();

	void setFileSystem(AFileSystem* fileSystem);

	AFileSystem* getFileSystem();

	AResourceManager<Image>* getImageManager();

	AResourceManager<ParticleEffectResource>* getParticleResourceManager();

	AResourceManager<Texture2D>* getTextureManager();

	lua::State* getLuastate();

	FontManager* getFontManager();

	ModelManager* getModelManager();

	AResourceManager<SkyBox>* getSkyBoxManager();

	AnimationManager* getAnimationManager();

	AResourceManager<ASoundBuffer>* getSoundManager();

	AResourceManager<Heightmap>* getHeightmapManager();

	AResourceManager<ScriptResource>* getScriptManager();

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

	struct ChunkedLoadStatus{
		uint32_t totalResources;
		uint32_t resourcesLoaded;
		bool finished;

		ChunkedLoadStatus() : totalResources(0), resourcesLoaded(0), finished(false){
		}
	};

	void chunkedLoadStart(const LuaObject& table);

	void chunkedLoadStart(const String& path);

	ChunkedLoadStatus chunkedLoad();

	void serialize(LuaObject& assets);

	void serialize(const String& path);

	void append(const LuaPlus::LuaObject& table);

	void append(const String& path);

private:
	class IChunkedLoader{
	public:
		virtual bool load() = 0;
		virtual uint32_t getTotalResources() = 0;
	};

	template<typename T>
	class ChunkedLoader : public IChunkedLoader{
	public:
		ChunkedLoader(AResourceManager<T>* manager) : mManager(manager){
			mIterator = mManager->getResourceIterator();
		}

		uint32_t getTotalResources(){
			return mManager->getNumResources();
		}

		bool load(){
			if(mIterator.next()){
				T* rsrc = *mIterator;

				if(!rsrc->isResourceLoaded() && mManager->getLoader()){
					if(rsrc->getUri().size()){
						Sp<AIOStream> stream = mManager->getResourceSystem()->getFileSystem()->open(rsrc->getUri(), AIOStream::eMODE_READ);

						try{
							mManager->load(stream, rsrc);
						}catch(...){
							LOGE("Error loading resource from uri \"%s\"", rsrc->getUri().c_str());
						}
					}
				}

				mIterator++;

				return true;
			}
			else{
				return false;
			}
		}

	private:
		AResourceManager<T>* mManager;
		typename AResourceManager<T>::ResourceIterator mIterator;
	};

	typedef std::vector<IChunkedLoader*> ChunkedLoaderList;

	struct ChunkedLoadState{
		bool inProgress;
		ChunkedLoaderList loaders;
		uint32_t totalResources;
		uint32_t currentResource;

		ChunkedLoadState() : inProgress(false), totalResources(0), currentResource(0){
		}
	} mChunkedLoadState;

	void init();

	Profiler mProfiler;

	AResourceManager<Image>* mImageManager;
	AResourceManager<Texture2D>* mTextureManager;
	ModelManager* mModelManager;
	AResourceManager<SkyBox>* mSkyBoxManager;
	FontManager* mFontManager;
	AnimationManager* mAnimationManager;
	SFSoundManager* mSoundManager;
	AFileSystem* mFileSystem;
	ParticleEffectResourceManager* mParticleManager;
	lua::State mLuaState;
	AResourceManager<Heightmap>* mHeightmapManager;
	AResourceManager<ScriptResource>* mScriptManager;

	template<class T>
	void serialize(AResourceManager<T>* manager, const String& name, lua::LuaObject& table);

	template<class T>
	void deserialize(AResourceManager<T>* manager, const String& name, const lua::LuaObject& table);

	void deserialize(const LuaObject& assets);

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
	if(!table.IsTable()){
		TRACEW("Error deserializing manager - Invalid managers table");
		return;
	}

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
