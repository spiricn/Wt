#include "stdafx.h"

#include "wte/WtEditorContext.h"

#include <wt/lua/LuaBindings.h>
#include <wt/SceneLoader.h>
#include <wt/FileSystemFactory.h>
#include <wt/Timer.h>

#define TD_TRACE_TAG "WtEditorContext"

#define WORKSPACE_TABLE_NAME "WORKSPACE"
#define SETTINGS_TABLE_NAME "SETTINGS"

WtEditorContext::WtEditorContext() : mAssetsFilePath(""), mSceneFilePath(""), mSceneLoaded(false), mAssetsLoaded(false), mLuaState(new wt::lua::State()),
	mEventManager(mLuaState), mScene(new wt::Physics(&mEventManager), &mAssets, &mEventManager, mLuaState), mRenderer(&mEventManager, &mScene){

	connect(&mTimer, SIGNAL(timeout()),
		this, SLOT(onTimeout()));
}

wt::ProcessManager* WtEditorContext::getProcManager(){
	return &mProcManager;
}

void WtEditorContext::onTimeout(){
	emit update(mStopwatch.getSeconds());
}

void WtEditorContext::startLoop(){
	mStopwatch.reset();

	mTimer.start( (1.0/25)*1000 );
}

void WtEditorContext::stopLoop(){
	if(mTimer.isActive()){
		mTimer.stop();
	}
}

wt::Assets* WtEditorContext::getAssets(){
	return &mAssets;
}

wt::lua::State* WtEditorContext::getLuaState(){
	return mLuaState;
}

wt::LuaObject WtEditorContext::getSettingsTable(){
	return mLuaState->getGlobals().Get(WORKSPACE_TABLE_NAME).Get(SETTINGS_TABLE_NAME);
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

	mAssetsFilePath = wt::utils::replacePathSplitters(path.toStdString(), '/').c_str();
	
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

//
//void WtEditorContext::switchWorkspace(const QString& path){
//	unloadAssets();
//
//	wt::AFileSystem::Desc fsDesc;
//	fsDesc.type = wt::AFileSystem::eTYPE_LOCAL;
//	fsDesc.dir.root = path.toStdString();
//
//	mAssets.setFileSystem( wt::FileSystemFactory::create(fsDesc) );
//
//	mWorkspacePath = path;
//
//	// Create new lua state
//	mLuaState = wt::lua::State();
//
//	//// Create workspace table
//	//WTE_CTX.getLuaState()->assignTable(workspace);
//	//WTE_CTX.getLuaState()->getGlobals().Set("workspace", workspace);
//
//	emit workspaceSwitched();
//}

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
	return mWorkspaceFilePath;
}

void WtEditorContext::saveScene(const QString& path){
	wt::SceneLoader loader(&mScene, &mAssets);

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	loader.save(stream);

	mSceneFilePath = path;
}

void WtEditorContext::saveAssets(const QString& path){
	mAssets.save(path.toStdString());

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
	TRACEW("Not implemented");
}

void WtEditorContext::clearAssets(){
	TRACEW("Not implemented");
}

void WtEditorContext::save(){
	// Notify everyone to save their settings
	emit saveRequest();

	saveAssets(mAssetsFilePath);

	saveScene(mSceneFilePath);
}

void WtEditorContext::createWorkspace(QString rootDir, QString filePath){
	unloadWorkspace();

	// Create a workspace table
	wt::LuaObject workspace = mLuaState->getGlobals().Get(WORKSPACE_TABLE_NAME);
	if(!workspace.IsTable()){
		mLuaState->assignTable(workspace);
		mLuaState->getGlobals().Set(WORKSPACE_TABLE_NAME, workspace);
	}

	// Create a settings table
	wt::LuaObject settings = workspace.Get(SETTINGS_TABLE_NAME);
	if(!settings .IsTable()){
		mLuaState->assignTable(settings );
		workspace.Set(SETTINGS_TABLE_NAME, settings );
	}

	mWorkspaceRoot = wt::utils::replacePathSplitters(rootDir.toStdString(), '/').c_str();

	saveWorkspace(filePath);

	emit workspaceLoaded();
}

void WtEditorContext::unloadWorkspace(){
	unloadAssets();

	mWorkspaceRoot = "";

	if(mLuaState){
		delete mLuaState;
	}

	mLuaState = new wt::lua::State();

		// Lua bindings
	wt::lua::LuaBindings_expose( getLuaState()->getGlobals() );

	// Setup scripting lua state
	mLuaState->expose(mScene, "Scene");
	mLuaState->expose(mEventManager, "EventManager");
	mLuaState->expose(mProcManager, "ProcessManager");
	
	emit workspaceUnloaded();
}

void WtEditorContext::loadWorkspace(QString filePath){
	unloadWorkspace();

	mLuaState->getStateOwner()->DoFile(filePath.toStdString().c_str());

	wt::LuaObject workspace = mLuaState->getGlobals().Get(WORKSPACE_TABLE_NAME);

	wt::String root, assets, scene;

	if(!wt::lua::luaConv(workspace.Get("root"), root)){
		LOGE("Invalid wtw file \"%s\"", filePath.toStdString().c_str());
		return;
	}

	// Create a file new system
	wt::AFileSystem::Desc fsDesc;
	fsDesc.type = wt::AFileSystem::eTYPE_LOCAL;
	fsDesc.dir.root = root;

	mAssets.setFileSystem( wt::FileSystemFactory::create(fsDesc) );

	// Load assets if necessary
	if(wt::lua::luaConv(workspace.Get("assets"), assets) && !assets.empty()){
		loadAssets(assets.c_str());
	}

	// Load scene if necessary
	if(wt::lua::luaConv(workspace.Get("scene"), scene) && !scene.empty()){
		loadScene(scene.c_str());
	}

	mWorkspaceFilePath = wt::utils::replacePathSplitters(filePath.toStdString(), '/').c_str();

	mWorkspaceRoot = wt::utils::replacePathSplitters(root, '/').c_str();

	emit workspaceLoaded();
}

void WtEditorContext::saveWorkspace(QString filePath){
	emit saveRequest();

	wt::LuaObject workspace = mLuaState->getGlobals().Get(WORKSPACE_TABLE_NAME);

	// Update workspace fields
	{
		workspace.Set("root", mWorkspaceRoot.toStdString().c_str());

		workspace.Set("scene", mSceneFilePath.toStdString().c_str());

		workspace.Set("assets", mAssetsFilePath.toStdString().c_str());
	}

	mWorkspaceFilePath = filePath;

	// Serialize
	wt::FileIOStream stream(filePath.toStdString(), wt::AIOStream::eMODE_WRITE);

	wt::lua::serializeTable(workspace, stream, WORKSPACE_TABLE_NAME);

	LOGI("Workspace saved to \"%s\"", filePath.toStdString().c_str());
}

QString WtEditorContext::getWorkspaceRootDir() const{
	return mWorkspaceRoot;
}