#ifndef WT_ASSETS_H
#define WT_ASSETS_H


#include "wt/stdafx.h"

#include "wt/FileIOStream.h"
#include "wt/Texture.h"
#include "wt/Material.h"
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
	Assets();

	~Assets();

	void setFileSystem(AFileSystem* fileSystem);

	AFileSystem* getFileSystem();

	AResourceManager<Image>* getImageManager();

	AResourceManager<ParticleEffectResource>* getParticleResourceManager();

	AResourceManager<Texture2D>* getTextureManager();

	lua::State* getLuastate();

	AResourceManager<Font>* getFontManager();

	AResourceManager<Model>* getModelManager();

	AResourceManager<SkyBox>* getSkyBoxManager();

	AResourceManager<Animation>* getAnimationManager();

	AResourceManager<ASoundBuffer>* getSoundManager();

	AResourceManager<Heightmap>* getHeightmapManager();

	AResourceManager<ScriptResource>* getScriptManager();

	//template<class T>
	//void load(AResourceManager<T>* manager);

	//template<class T>
	//void create(AResourceManager<T>* manager);

	ASoundSystem* getSoundSystem();

	void unloadAll();

	String getRelativeURI(const String& uri);

	virtual void reload(const String& set="");

	virtual void unload(const String& set="");

	virtual void load(const LuaPlus::LuaObject& table, const String& set="");

	virtual void load(const String& path, const String& set="");

	virtual void save(LuaPlus::LuaObject& assets, const String& set="");

	virtual void save(const String& path, const String& set="");

	void reloadAll();

	void addToSet(IResource* resource, const String& set);

	IResourceManager* getManager(IResource::ResourceType type);

private:

	void init();

	//template<class T>
	//void serialize(AResourceManager<T>* manager, const String& name, lua::LuaObject& table);

	//template<class T>
	//void deserialize(AResourceManager<T>* manager, const String& name, const lua::LuaObject& table);

	//void deserialize(const LuaObject& assets);


	typedef std::map<IResource::ResourceType, IResourceSet*> ResourceTypeSetMap;
	typedef std::map<String, ResourceTypeSetMap*> ResourceSetMap;

	struct RegisteredManager{
		IResourceManager* manager;
		String tableName;

		RegisteredManager(IResourceManager* manager, String tableName) : manager(manager), tableName(tableName){
		}
	}; // </RegisteredManager>
	
	typedef std::set<RegisteredManager*> ManagerSet;

private:

	ResourceTypeSetMap* getSetMap(const String& name);

	IResourceSet* getSet(IResourceManager* manager, const String& name);

private:

	ManagerSet mManagers;
	ResourceSetMap mResourceSets;
	Profiler mProfiler;
	AResourceManager<Image>* mImageManager;
	AResourceManager<Texture2D>* mTextureManager;
	AResourceManager<Model>* mModelManager;
	AResourceManager<SkyBox>* mSkyBoxManager;
	AResourceManager<Font>* mFontManager;
	AResourceManager<Animation>* mAnimationManager;
	SFSoundManager* mSoundManager;
	AFileSystem* mFileSystem;
	AResourceManager<ParticleEffectResource>* mParticleManager;
	lua::State mLuaState;
	AResourceManager<Heightmap>* mHeightmapManager;
	AResourceManager<ScriptResource>* mScriptManager;
}; // </Assets>

//#define TD_TRACE_TAG "Assets"
//
//template<class T>
//void Assets::load(AResourceManager<T>* manager){
//	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
//		T* r = *i;
//		manager->getLoader()->load(r->getUri(), r);
//		mLoadedResources++;
//		mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
//	}
//}
//
//template<class T>
//void Assets::create(AResourceManager<T>* manager){
//	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
//		T* r = *i;
//		manager->getLoader()->create(r);
//		mLoadedResources++;
//
//		mListener->onLoadProgress((float)mLoadedResources/mTotalResources);
//	}
//}
//
//
//template<class T>
//void Assets::serialize(AResourceManager<T>* manager, const String& name, lua::LuaObject& table){
//
//	fixAbsolutePaths(this, manager);
//
//	lua::LuaObject managerTable = getLuastate()->newTable();
//	table.Set(name.c_str(), managerTable);
//
//	lua::LuaObject root = getLuastate()->newTable();
//	managerTable.Set("$ROOT", root);
//
//	manager->serialize(&mLuaState, root);
//}
//
//
//template<class T>
//void Assets::deserialize(AResourceManager<T>* manager, const String& name, const lua::LuaObject& table){
//	if(!table.IsTable()){
//		TRACEW("Error deserializing manager - Invalid managers table");
//		return;
//	}
//
//	lua::LuaObject mngrTable = table.Get(name.c_str());
//
//	if(!mngrTable.IsTable()){
//		LOGW("Manager table \"%s\" not found - skipping deserialization", name.c_str());
//		return;
//	}
//
//	manager->deserialize(&mLuaState, mngrTable.Get("$ROOT"));
//}

#define TD_TRACE_TAG ""

}; // </wt>

#endif
