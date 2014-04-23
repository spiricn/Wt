#include "wt/stdafx.h"

#include "wt/Assets.h"
#include "wt/Texture2DLoader.h"

#define TD_TRACE_TAG "Assets"

#define IMAGE_MANAGER_TABLE_NAME "IMAGE_MANAGER"
#define TEXTURE_MANAGER_TABLE_NAME "TEXTURE_MANAGER"
#define SKYBOX_MANAGER_TABLE_NAME "SKYBOX_MANAGER"
#define ANIMATION_MANAGER_TABLE_NAME "ANIMATION_MANAGER"
#define MODEL_MANAGER_TABLE_NAME "MODEL_MANAGER"
#define SOUND_MANAGER_TABLE_NAME "SOUND_MANAGER"
#define PARTICLE_MANAGER_TABLE_NAME "PARTICLE_MANAGER"
#define HEIGHTMAP_MANAGER_TABLE_NAME "HEIGHTMAP_MANAGER"
#define SCRIPT_MANAGER_TABLE_NAME "SCRIPT_MANAGER"
#define FONT_MANAGER_TABLE_NAME "FONT_MANAGER"
#define ASSETS_TABLE_NAME "ASSETS"

namespace wt
{


template<class T>
void fixAbsolutePaths(Assets* thiz, AResourceManager<T>* manager){
	for(AResourceManager<T>::ResourceIterator i=manager->getResourceIterator(); i.next(); i++){
		i->setUri(
			thiz->getRelativeURI(i->getUri())
		);
	}
}


Assets::Assets() : mFileSystem(NULL){
	init();
}

void Assets::init(){
	mImageManager = new AResourceManager<Image>(this, IResource::eTYPE_IMAGE);
	mManagers.insert(new RegisteredManager(mImageManager, IMAGE_MANAGER_TABLE_NAME));

	mTextureManager = new AResourceManager<Texture2D>(this, IResource::eTYPE_TEXTURE);
	mManagers.insert(new RegisteredManager(mTextureManager, TEXTURE_MANAGER_TABLE_NAME));

	mAnimationManager = new AResourceManager<Animation>(this, IResource::eTYPE_ANIMATION);
	mManagers.insert(new RegisteredManager(mAnimationManager, ANIMATION_MANAGER_TABLE_NAME));

	mModelManager = new AResourceManager<Model>(this, IResource::eTYPE_MODEL);
	mManagers.insert(new RegisteredManager(mModelManager, MODEL_MANAGER_TABLE_NAME));

	mSkyBoxManager = new AResourceManager<SkyBox>(this, IResource::eTYPE_SKYBOX);
	mManagers.insert(new RegisteredManager(mSkyBoxManager, SKYBOX_MANAGER_TABLE_NAME));

	mSoundManager = new SFSoundManager(this);
	mManagers.insert(new RegisteredManager(mSoundManager, SOUND_MANAGER_TABLE_NAME));

	mParticleManager = new AResourceManager<ParticleEffectResource>(this, IResource::eTYPE_PARTICLE);
	mManagers.insert(new RegisteredManager(mParticleManager, PARTICLE_MANAGER_TABLE_NAME));

	mHeightmapManager = new AResourceManager<Heightmap>(this, IResource::eTYPE_HEIGHTMAP);
	mManagers.insert(new RegisteredManager(mHeightmapManager, HEIGHTMAP_MANAGER_TABLE_NAME));

	mFontManager = new AResourceManager<Font>(this, IResource::eTYPE_FONT);
	mManagers.insert(new RegisteredManager(mFontManager, FONT_MANAGER_TABLE_NAME));

	mScriptManager = new AResourceManager<ScriptResource>(this, IResource::eTYPE_SCRIPT);
	mManagers.insert(new RegisteredManager(mScriptManager, SCRIPT_MANAGER_TABLE_NAME));

	mImageManager->setLoader( &DevilImageLoader::getSingleton() );
	mTextureManager->setLoader( &Texture2DLoader::getSingleton() );
	mSkyBoxManager->setLoader( &SkyBoxLoader::getSingleton() );
	mModelManager->setLoader( &ModelLoader::getSingleton() );
	mAnimationManager->setLoader( &AnimationLoader::getSingleton() );
	mSoundManager->setLoader( &SFSoundLoader::getSingleton() );
	mParticleManager->setLoader( NULL );
	mHeightmapManager->setLoader( &HeightmapLoader::getSingleton() );
	mScriptManager->setLoader( &ScriptLoader::getSingleton() );
}

template<class T>
void deserializeSet(wt::lua::State* luaState, AResourceManager<T>* manager, const String& name, const lua::LuaObject& table, typename AResourceGroup<T>::ResourceSet& set){
	if(!table.IsTable()){
		TRACEW("Error deserializing manager - Invalid managers table");
		return;
	}

	lua::LuaObject mngrTable = table.Get(name.c_str());

	if(!mngrTable.IsTable()){
		LOGW("Manager table \"%s\" not found - skipping deserialization", name.c_str());
		return;
	}

	manager->deserialize(luaState, mngrTable.Get("$ROOT"), set);
}
//
//void Assets::append(const LuaPlus::LuaObject& table){
//	// Crete & deserialize new resources
//	AResourceGroup<Image>::ResourceSet imageSet;
//	deserializeSet(&mLuaState, mImageManager, IMAGE_MANAGER_TABLE_NAME, table, imageSet);
//
//	AResourceGroup<Texture2D>::ResourceSet textureSet;
//	deserializeSet(&mLuaState, mTextureManager, TEXTURE_MANAGER_TABLE_NAME, table, textureSet);
//
//	AResourceGroup<SkyBox>::ResourceSet skyboxSet;
//	deserializeSet(&mLuaState, mSkyBoxManager, SKYBOX_MANAGER_TABLE_NAME, table, skyboxSet);
//
//	AResourceGroup<Animation>::ResourceSet animationSet;
//	deserializeSet(&mLuaState, mAnimationManager, ANIMATION_MANAGER_TABLE_NAME, table, animationSet);
//
//	AResourceGroup<Model>::ResourceSet modelSet;
//	deserializeSet(&mLuaState, mModelManager, MODEL_MANAGER_TABLE_NAME, table, modelSet);
//
//	AResourceGroup<ASoundBuffer>::ResourceSet soundSet;
//	deserializeSet(&mLuaState, mSoundManager, SOUND_MANAGER_TABLE_NAME, table, soundSet);
//
//	AResourceGroup<ParticleEffectResource>::ResourceSet particleSet;
//	deserializeSet(&mLuaState, mParticleManager, PARTICLE_MANAGER_TABLE_NAME, table, particleSet);
//
//	AResourceGroup<Heightmap>::ResourceSet heightmapSet;
//	deserializeSet(&mLuaState, mHeightmapManager, HEIGHTMAP_MANAGER_TABLE_NAME, table, heightmapSet);
//
//	AResourceGroup<ScriptResource>::ResourceSet scriptSet;
//	deserializeSet(&mLuaState, mScriptManager, SCRIPT_MANAGER_TABLE_NAME, table, scriptSet);
//
//	// Load new resources
//	mImageManager->loadSet(imageSet);
//	mTextureManager->loadSet(textureSet);
//	mSkyBoxManager->loadSet(skyboxSet);
//	mAnimationManager->loadSet(animationSet);
//	mModelManager->loadSet(modelSet);
//	mSoundManager->loadSet(soundSet);
//	mParticleManager->loadSet(particleSet);
//	mHeightmapManager->loadSet(heightmapSet);
//	mScriptManager->loadSet(scriptSet);
//
//	// Create new resources
//	mImageManager->createSet(imageSet);
//	mTextureManager->createSet(textureSet);
//	mSkyBoxManager->createSet(skyboxSet);
//	mAnimationManager->createSet(animationSet);
//	mModelManager->createSet(modelSet);
//	mSoundManager->createSet(soundSet);
//	mParticleManager->createSet(particleSet);
//	mHeightmapManager->createSet(heightmapSet);
//	mScriptManager->createSet(scriptSet);
//}

//void Assets::deserialize(const LuaObject& assets){
//	deserialize(mImageManager, IMAGE_MANAGER_TABLE_NAME, assets);
//	deserialize(mTextureManager, TEXTURE_MANAGER_TABLE_NAME, assets);
//	deserialize(mSkyBoxManager, SKYBOX_MANAGER_TABLE_NAME, assets);
//	deserialize(mAnimationManager, ANIMATION_MANAGER_TABLE_NAME, assets);
//	deserialize(mModelManager, MODEL_MANAGER_TABLE_NAME, assets);
//	deserialize(mSoundManager, SOUND_MANAGER_TABLE_NAME, assets);
//	deserialize(mParticleManager, PARTICLE_MANAGER_TABLE_NAME, assets);
//	deserialize(mHeightmapManager, HEIGHTMAP_MANAGER_TABLE_NAME, assets);
//	deserialize(mScriptManager, SCRIPT_MANAGER_TABLE_NAME, assets);
//}

AFileSystem* Assets::getFileSystem(){
	return mFileSystem;
}

AResourceManager<Image>* Assets::getImageManager(){
	return mImageManager;
}

void Assets::setFileSystem(AFileSystem* fileSystem){
	mFileSystem = fileSystem;
}

AResourceManager<ParticleEffectResource>* Assets::getParticleResourceManager(){
	return mParticleManager;
}

AResourceManager<ScriptResource>* Assets::getScriptManager(){
	return mScriptManager;
}

AResourceManager<Texture2D>* Assets::getTextureManager(){
	return mTextureManager;
}

lua::State* Assets::getLuastate(){
	return &mLuaState;
}

AResourceManager<Font>* Assets::getFontManager(){
	return mFontManager;
}

AResourceManager<Model>* Assets::getModelManager(){
	return mModelManager;
}

AResourceManager<Heightmap>* Assets::getHeightmapManager(){
	return mHeightmapManager;
}

AResourceManager<SkyBox>* Assets::getSkyBoxManager(){
	return mSkyBoxManager;
}

AResourceManager<Animation>* Assets::getAnimationManager(){
	return mAnimationManager;
}

AResourceManager<ASoundBuffer>* Assets::getSoundManager(){
	return mSoundManager;
}

ASoundSystem* Assets::getSoundSystem(){
	return mSoundManager;
}

void Assets::unloadAll(){
	for(ResourceSetMap::iterator i=mResourceSets.begin(); i!=mResourceSets.end(); i++){
		unload(i->first);
	}
}

void Assets::reload(const String& setName){
	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		IResourceSet* set = getSet((*iter)->manager, setName);

		(*iter)->manager->loadSet(*set);
	}

	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		IResourceSet* set = getSet((*iter)->manager, setName);

		(*iter)->manager->createSet(*set);
	}
}

String Assets::getRelativeURI(const String& uri){
	return utils::toRelative(mFileSystem->getRoot(), uri);
}

Assets::ResourceTypeSetMap* Assets::getSetMap(const String& name){
	ResourceTypeSetMap* res = NULL;

	ResourceSetMap::iterator iter = mResourceSets.find(name);

	if(iter == mResourceSets.end()){
		// Create new set
		mResourceSets.insert( std::make_pair(name, res = new ResourceTypeSetMap) );

		// Create a set for each resource type
		for(int32_t i=0; i<IResource::eTYPE_MAX; i++){
			res->insert(std::make_pair(static_cast<IResource::ResourceType>(i), new IResourceSet));
		}
	}
	else{
		res = iter->second;
	}

	return res;
}

IResourceSet* Assets::getSet(IResourceManager* manager, const String& name){
	ResourceTypeSetMap* sets = getSetMap(name);

	return sets->find(manager->getResourceType())->second;
}

void Assets::load(const LuaObject& table, const String& setName){
	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		RegisteredManager* rm = *iter;

		IResourceSet* set = getSet(rm->manager, setName);

		LuaObject managerTable = table.Get(rm->tableName.c_str());

		if(!managerTable.IsTable()){
			LOGW("Manager table \"%s\" not found - skipping deserialization", rm->tableName.c_str());
			continue;
		}

		rm->manager->deserialize(&mLuaState, managerTable.Get("$ROOT"), *set);
	}

	reload(setName);
}

void Assets::load(const String& path, const String& set){
	LuaPlus::LuaStateOwner state;
	WT_ASSERT(mFileSystem, "No file system set");

	String relativePath = utils::toRelative(mFileSystem->getRoot(), path);

	Sp<AIOStream> stream = mFileSystem->open(relativePath, AIOStream::eMODE_READ);
	try{
		lua::doStream(state, *(stream.get()));
	}catch(...){
		LOGE("Error executing asset script");
		throw;
	}
	LuaObject assets = state->GetGlobal("ASSETS");

	load(assets);
}


void Assets::save(LuaObject& table, const String& setName){
	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		RegisteredManager* rm = *iter;

		IResourceSet* set = getSet(rm->manager, setName);

		// Create a manager table
		lua::LuaObject managerTable = getLuastate()->newTable();
		table.Set(rm->tableName.c_str(), managerTable);

		// Create a root group table
		lua::LuaObject root = getLuastate()->newTable();
		managerTable.Set("$ROOT", root);

		// Serialize
		rm->manager->serialize(&mLuaState, root, *set);
	}

	// TODO figure out a better way of doing this
	fixAbsolutePaths(this, mImageManager);
	fixAbsolutePaths(this, mTextureManager);
	fixAbsolutePaths(this, mAnimationManager);
	fixAbsolutePaths(this, mModelManager);
	fixAbsolutePaths(this, mSkyBoxManager);
	fixAbsolutePaths(this, mSoundManager);
	fixAbsolutePaths(this, mParticleManager);
	fixAbsolutePaths(this, mHeightmapManager);
	fixAbsolutePaths(this, mFontManager);
	fixAbsolutePaths(this, mScriptManager);
}

void Assets::save(const String& path, const String& set){
	lua::LuaObject assets = getLuastate()->newTable();

	save(assets, set);

	WT_ASSERT(mFileSystem, "No file system set");

	String relativePath = utils::toRelative(mFileSystem->getRoot(), path);

	StreamPtr stream = mFileSystem->open(relativePath, AIOStream::eMODE_WRITE);

	lua::serializeTable(assets, *stream.get(), ASSETS_TABLE_NAME );
}

Assets::~Assets(){
	if(mFileSystem){
		delete mFileSystem;
	}

	for(ResourceSetMap::iterator i=mResourceSets.begin(); i!=mResourceSets.end(); i++){
		for(ResourceTypeSetMap::iterator j=i->second->begin(); j!=i->second->end(); j++){
			delete j->second;
		}

		delete i->second;
	}

	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		delete (*iter)->manager;
	}
}

void Assets::reloadAll(){
}

void Assets::unload(const String& setName){
	for(ManagerSet::iterator iter=mManagers.begin(); iter!=mManagers.end(); iter++){
		RegisteredManager* rm = *iter;

		IResourceSet* set = getSet(rm->manager, setName);

		rm->manager->destroySet(*set);

		set->clear();
	}
}

IResourceManager* Assets::getManager(IResource::ResourceType type){
	switch(type){
	case IResource::eTYPE_ANIMATION:
		return mAnimationManager;
	case IResource::eTYPE_FONT:
		return mFontManager;
	case IResource::eTYPE_HEIGHTMAP:
		return mHeightmapManager;
	case IResource::eTYPE_IMAGE:
		return mImageManager;
	case IResource::eTYPE_MODEL:
		return mModelManager;
	case IResource::eTYPE_PARTICLE:
		return mParticleManager;
	case IResource::eTYPE_SCRIPT:
		return mScriptManager;
	case IResource::eTYPE_SKYBOX:
		return mSkyBoxManager;
	case IResource::eTYPE_TEXTURE:
		return mTextureManager;
	case IResource::eTYPE_SOUND:
		return mSoundManager;
	default:
		WT_THROW("Unhandled resource type %d", type);
	}
}

void Assets::addToSet(IResource* resource, const String& set){
	IResourceManager* manager = getManager(resource->getResourceType());

	getSet(manager, set)->insert(resource);
}

} // </wt>