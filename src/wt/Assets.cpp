#include "wt/stdafx.h"

#include "wt/Assets.h"

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

namespace wt
{

Assets::Assets() : mFileSystem(NULL){
	init();
}

void Assets::init(){
	mImageManager = new ImageManager(this);
	mTextureManager = new TextureManager(this);
	mAnimationManager = new AnimationManager(this);
	mModelManager = new ModelManager(this);
	mSkyBoxManager = new SkyBoxManager(this);
	mSoundManager = new SFSoundManager(this);
	mParticleManager = new ParticleEffectResourceManager(this);
	mHeightmapManager = new AResourceManager<Heightmap>(this);
	mFontManager = new FontManager(this);
	mScriptManager = new AResourceManager<ScriptResource>(this);

	mImageManager->setLoader( &DevilImageLoader::getSingleton() );
	mTextureManager->setLoader( &TextureLoader::getSingleton() );
	mSkyBoxManager->setLoader( &SkyBoxLoader::getSingleton() );
	mModelManager->setLoader( &ModelLoader::getSingleton() );
	mAnimationManager->setLoader( &AnimationLoader::getSingleton() );
	mSoundManager->setLoader( &SFSoundLoader::getSingleton() );
	mParticleManager->setLoader( NULL );
	mHeightmapManager->setLoader( &HeightmapLoader::getSingleton() );
	mScriptManager->setLoader( &ScriptLoader::getSingleton() );
}

void Assets::deserialize(const LuaObject& assets){
	deserialize(mImageManager, IMAGE_MANAGER_TABLE_NAME, assets);
	deserialize(mTextureManager, TEXTURE_MANAGER_TABLE_NAME, assets);
	deserialize(mSkyBoxManager, SKYBOX_MANAGER_TABLE_NAME, assets);
	deserialize(mAnimationManager, ANIMATION_MANAGER_TABLE_NAME, assets);
	deserialize(mModelManager, MODEL_MANAGER_TABLE_NAME, assets);
	deserialize(mSoundManager, SOUND_MANAGER_TABLE_NAME, assets);
	deserialize(mParticleManager, PARTICLE_MANAGER_TABLE_NAME, assets);
	deserialize(mHeightmapManager, HEIGHTMAP_MANAGER_TABLE_NAME, assets);
	deserialize(mScriptManager, SCRIPT_MANAGER_TABLE_NAME, assets);
}

AFileSystem* Assets::getFileSystem(){
	return mFileSystem;
}

ImageManager* Assets::getImageManager(){
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

TextureManager* Assets::getTextureManager(){
	return mTextureManager;
}

lua::State* Assets::getLuastate(){
	return &mLuaState;
}

FontManager* Assets::getFontManager(){
	return mFontManager;
}

ModelManager* Assets::getModelManager(){
	return mModelManager;
}

AResourceManager<Heightmap>* Assets::getHeightmapManager(){
	return mHeightmapManager;
}

SkyBoxManager* Assets::getSkyBoxManager(){
	return mSkyBoxManager;
}

AnimationManager* Assets::getAnimationManager(){
	return mAnimationManager;
}

AResourceManager<ASoundBuffer>* Assets::getSoundManager(){
	return mSoundManager;
}

ASoundSystem* Assets::getSoundSystem(){
	return mSoundManager;
}

void Assets::unloadAll(){
	LOGD("Unloading assets..");

	mImageManager->destroy();

	mTextureManager->destroy();

	mSkyBoxManager->destroy();

	mAnimationManager->destroy();

	mModelManager->destroy();

	mSoundManager->destroy();

	mFontManager->destroy();

	mParticleManager->destroy();

	mHeightmapManager->destroy();

	mScriptManager->destroy();

	LOGD("Done..");
}

void Assets::chunkedLoadStart(const String& path){
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

	chunkedLoadStart(assets);
}

void Assets::chunkedLoadStart(const LuaObject& table){
	WT_ASSERT(!mChunkedLoadState.inProgress, "chunkedLoadStart already called");

	deserialize(table);

	mChunkedLoadState.inProgress = true;

	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Image>(mImageManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Texture2D>(mTextureManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<SkyBox>(mSkyBoxManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Animation>(mAnimationManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Model>(mModelManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<ASoundBuffer>(mSoundManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Font>(mFontManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<ParticleEffectResource>(mParticleManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<Heightmap>(mHeightmapManager) );
	mChunkedLoadState.loaders.push_back( new ChunkedLoader<ScriptResource>(mScriptManager) );

	for(ChunkedLoaderList::iterator iter=mChunkedLoadState.loaders.begin(); iter!=mChunkedLoadState.loaders.end(); iter++){
		mChunkedLoadState.totalResources += (*iter)->getTotalResources();
	}

	LOGD("Starting chunked load; totalResources=%u", mChunkedLoadState.totalResources);
}

Assets::ChunkedLoadStatus Assets::chunkedLoad(){
	WT_ASSERT(mChunkedLoadState.inProgress, "chunkedLoadStart not called");

	ChunkedLoadStatus stat;

	IChunkedLoader* currentLoader = mChunkedLoadState.loaders[0];

	if(currentLoader->load()){
		stat.resourcesLoaded = ++mChunkedLoadState.currentResource;
		stat.totalResources = mChunkedLoadState.totalResources;

		return stat;
	}
	else{
		mChunkedLoadState.loaders.erase( mChunkedLoadState.loaders.begin() );

		// Reached the end
		if(mChunkedLoadState.loaders.empty()){
			stat.finished = true;
			mChunkedLoadState.inProgress = false;

			// TODO move this elsewhere
			mImageManager->createAll();
			mTextureManager->createAll();
			mSkyBoxManager->createAll();
			mAnimationManager->createAll();
			mSoundManager->createAll();
			mModelManager->createAll();
			mParticleManager->createAll();
			mHeightmapManager->createAll();
			mScriptManager->createAll();
		}
		else{
			return chunkedLoad();
		}
	}

	return stat;
}

void Assets::reload(){
	LOGD("Loading images . . .");
	mImageManager->loadAll();

	LOGD("Loading textures . . .");
	mTextureManager->loadAll();

	LOGD("Loading skyboxes . . .");
	mSkyBoxManager->loadAll();

	LOGD("Loading animations . . .");
	mAnimationManager->loadAll();

	LOGD("Loading models . . .");
	mModelManager->loadAll();

	LOGD("Loading sounds . . .");
	mSoundManager->loadAll();

	LOGD("Loading particles . . .");
	mParticleManager->loadAll();

	LOGD("Loading heightmaps . . .");
	mHeightmapManager->loadAll();

	LOGD("Loading scripts . . .");
	mScriptManager->loadAll();

	LOGD("Creating resources");

	mImageManager->createAll();

	mTextureManager->createAll();

	mSkyBoxManager->createAll();

	mAnimationManager->createAll();

	mSoundManager->createAll();

	mModelManager->createAll();

	mParticleManager->createAll();

	mHeightmapManager->createAll();

	mScriptManager->createAll();

	LOGD("All assets created.");
}

String Assets::getRelativeURI(const String& uri){
	return utils::toRelative(mFileSystem->getRoot(), uri);
}

void Assets::load(const LuaObject& table){
	deserialize(table);

	reload();
}

void Assets::load(const String& path){
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


void Assets::serialize(LuaObject& assets){
	serialize(mImageManager, IMAGE_MANAGER_TABLE_NAME,
		assets);

	serialize(mTextureManager, TEXTURE_MANAGER_TABLE_NAME, 
		assets);

	serialize(mSkyBoxManager, SKYBOX_MANAGER_TABLE_NAME, 
		assets);

	serialize(mModelManager, MODEL_MANAGER_TABLE_NAME, 
		assets);

	serialize(mAnimationManager, ANIMATION_MANAGER_TABLE_NAME, 
		assets);

	serialize(mSoundManager, SOUND_MANAGER_TABLE_NAME,
		assets);

	serialize(mParticleManager, PARTICLE_MANAGER_TABLE_NAME,
		assets);

	serialize(mHeightmapManager, HEIGHTMAP_MANAGER_TABLE_NAME,
		assets);

	serialize(mScriptManager, SCRIPT_MANAGER_TABLE_NAME,
		assets);
}

void Assets::serialize(const String& path){
	lua::LuaObject assets = getLuastate()->newTable();

	serialize(assets);

	WT_ASSERT(mFileSystem, "No file system set");

	String relativePath = utils::toRelative(mFileSystem->getRoot(), path);

	StreamPtr stream = mFileSystem->open(relativePath, AIOStream::eMODE_WRITE);

	stream->print("ASSETS=");
	lua::serializeTable(assets, *stream.get());
}

Assets::~Assets(){
	delete mImageManager;

	delete mTextureManager;

	delete mModelManager;

	delete mSkyBoxManager;

	delete mAnimationManager;

	if(mFileSystem){
		delete mFileSystem;
	}

	delete mParticleManager;

	delete mSoundManager;

	delete mHeightmapManager;

	delete mFontManager;

	delete mScriptManager;
}

} // </wt>