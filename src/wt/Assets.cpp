#include "wt/stdafx.h"

#include "wt/Assets.h"

#define TD_TRACE_TAG "Assets"

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

	// TODO remvoe singleton
	mFontManager = &FontManager::getSingleton();

	mImageManager->setLoader( &DevilImageLoader::getSingleton() );
	mTextureManager->setLoader( &TextureLoader::getSingleton() );
	mSkyBoxManager->setLoader( &SkyBoxLoader::getSingleton() );
	mModelManager->setLoader( &ModelLoader::getSingleton() );
	mAnimationManager->setLoader( &AnimationLoader::getSingleton() );
	mSoundManager->setLoader( &SFSoundLoader::getSingleton() );
	mParticleManager->setLoader( NULL );
}

void Assets::deserialize(const LuaObject& assets){
	deserialize(mImageManager, "IMAGE_MANAGER", assets);
	deserialize(mTextureManager, "TEXTURE_MANAGER", assets);
	deserialize(mSkyBoxManager, "SKYBOX_MANAGER", assets);
	deserialize(mAnimationManager, "ANIMATION_MANAGER", assets);
	deserialize(mModelManager, "MODEL_MANAGER", assets);
	deserialize(mSoundManager, "SOUND_MANAGER", assets);
	deserialize(mParticleManager, "PARTICLE_MANAGER", assets);
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

	LOGD("Done..");
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

	LOGD("Creating resources");

	mImageManager->createAll();
	mTextureManager->createAll();
	mSkyBoxManager->createAll();
	mAnimationManager->createAll();
	mSoundManager->createAll();
	mModelManager->createAll();
	mParticleManager->createAll();

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
#if 0
	if(state->DoFile(path.c_str())){
		WT_THROW("Error executing asset script");
	}
#else
	WT_ASSERT(mFileSystem, "No file system set");

	String relativePath = utils::toRelative(mFileSystem->getRoot(), path);

	Sp<AIOStream> stream = mFileSystem->open(relativePath, AIOStream::eMODE_READ);
	try{
		lua::doStream(state, *(stream.get()));
	}catch(...){
		LOGE("Error executing asset script");
		throw;
	}
#endif

	LuaObject assets = state->GetGlobal("ASSETS");

	load(assets);
}


void Assets::serialize(LuaObject& assets){
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
	serialize(mParticleManager, "PARTICLE_MANAGER",
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
}

} // </wt>