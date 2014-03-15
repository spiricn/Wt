#include "stdafx.h"

#include "wte/WtEditorContext.h"

#include <wt/SceneLoader.h>
#include <wt/FileSystemFactory.h>

#define TD_TRACE_TAG "WtEditorContext"

WtEditorContext::WtEditorContext() : mAssetsFilePath(""), mSceneFilePath(""), mSceneLoaded(false), mAssetsLoaded(false),
	mEventManager(&mLuaState), mScene(new wt::Physics(&mEventManager), &mAssets, &mEventManager, &mLuaState), mRenderer(&mEventManager, &mScene){
		mLuaState.assignTable(mSettingsTable);
}

wt::Assets* WtEditorContext::getAssets(){
	return &mAssets;
}

wt::lua::State* WtEditorContext::getLuaState(){
	return &mLuaState;
}

wt::LuaObject WtEditorContext::getSettingsTable(){
	return mSettingsTable;
}

void WtEditorContext::loadScene(const QString& path){
	LOGD("Loading scene from file \"%s\"", path.toStdString().c_str());

	// Unload existing scene
	unloadScene();

	try{
		wt::SceneLoader loader(&mScene, &mAssets);

		wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_READ);

		loader.load(stream);

		mSceneFilePath = path;
	}catch(...){
		TRACEE("Error loading scene");
		mScene.clear();
		return;
	}

	emit sceneLoaded();

	mSceneLoaded = true;

	LOGD("Scene loaded OK");
}

void WtEditorContext::unloadScene(){
	if(!mSceneLoaded){
		return;
	}

	LOGD("Unloading scene ..");

	mSceneFilePath = "";

	mScene.clear();
	
	mSceneLoaded = false;

	emit sceneUnloaded();

	LOGD("Scene unloaded OK");
}

void WtEditorContext::loadAssets(const QString& path){
	LOGD("Loading resources from file \"%s\"", path.toStdString().c_str());

	// Unload existing resources
	unloadAssets();

	mAssetsFilePath = path;
	
	LuaPlus::LuaStateOwner state;
	//try{
		state->DoFile(path.toStdString().c_str());
		mAssets.load( state->GetGlobal("ASSETS") );
	// TODO move this catch to WtEditor
	//}catch(...){
		//QMessageBox::critical(this, "Error", "Error loading assets from \"" + path + "\"");
		//return;
	//}

	mAssetsLoaded = true;


	// TODO handle this in WTEditor via a slot
	//// Destroy all resource tab items & unload all resources
	//for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
	//	(*i)->refreshAll();
	//}

	emit assetsLoaded();

	LOGD("Resources loaded OK");
}

wt::EventManager* WtEditorContext::getEventManager(){
	return &mEventManager;
}

wt::Scene* WtEditorContext::getScene(){
	return &mScene;
}

wt::Renderer* WtEditorContext::getRenderer(){
	return &mRenderer;
}

void WtEditorContext::switchWorkspace(const QString& path){
	unloadAssets();

	wt::AFileSystem::Desc fsDesc;
	fsDesc.type = wt::AFileSystem::eTYPE_LOCAL;
	fsDesc.dir.root = path.toStdString();

	mAssets.setFileSystem( wt::FileSystemFactory::create(fsDesc) );

	mWorkspacePath = path;

	emit workspaceSwitched();
}

void WtEditorContext::unloadAssets(){
	if(!mAssetsLoaded){
		return;
	}

	LOGD("Unloading resources ..");

	// Unload existing scene
	unloadScene();

	mAssets.unloadAll();

	mAssetsFilePath = "";

	mAssetsLoaded = false;

	emit assetsUnloaded();

	LOGD("Resource unloaded OK");
}


bool WtEditorContext::isAssetsLoaded() const{
	return mAssetsLoaded;
}

bool WtEditorContext::isSceneLoaded() const{
	return mSceneLoaded;
}

QString WtEditorContext::getAssetsFilePath() const{
	return mAssetsFilePath;
}

QString WtEditorContext::getSceneFilePath() const{
	return mSceneFilePath;
}

QString WtEditorContext::getWorkspaceFilePath() const{
	return mWorkspacePath;
}

void WtEditorContext::saveScene(const QString& path){
	wt::SceneLoader loader(&mScene, &mAssets);

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	loader.save(stream);

	mSceneFilePath = path;
}

void WtEditorContext::saveAssets(const QString& path){
	mAssets.serialize(path.toStdString());

	mAssetsFilePath = path;
}

void WtEditorContext::reloadAssets(){
	LOGD("Reloading everything ...");

	QString scenePath = mSceneFilePath;

	QString assetsPath = mAssetsFilePath;

	bool sceneLoaded = mSceneLoaded;

	// Unload resources / scene
	unloadAssets();

	loadAssets(assetsPath);

	// Load scene if there was one
	if(sceneLoaded){
		loadScene(scenePath);
	}

	LOGD("Everything reloaded OK");
}

void WtEditorContext::createNewScene(QString path){
	if(!path.size()){
		return;
	}

	// Unload existing scene
	WTE_CTX.unloadScene();

	// TODO move catch to WtEditor
	//try{
		WTE_CTX.saveScene(path);
		mSceneFilePath = path;
		mSceneLoaded = true;
	/*}catch(...){
		TRACEE("Error creating new assets file");
		return;
	}*/
}


void WtEditorContext::createNewAssets(QString path){
	if(!path.size()){
		return;
	}

	// Unload existing assets & scene
	WTE_CTX.unloadAssets();

	// TODO mvoe catch to WtEditor
	//try{
		// Create an empty assets file
		WTE_CTX.saveAssets( path );
	//}catch(...){
	//	TRACEE("Error creating new assets file");
	//	return;
	//}
}

void WtEditorContext::clearScene(){
}

void WtEditorContext::clearAssets(){
}