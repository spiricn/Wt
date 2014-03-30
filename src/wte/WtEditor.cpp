#include "stdafx.h"

#include <wt/Utils.h>
#include <wt/SceneLoader.h>
#include <wt/FileSystemFactory.h>
#include <wt/Lua.h>

#include "wte/ModelImporterTab.h"
#include "wte/FilePicker.h"
#include "wte/WtEditor.h"
#include "wte/Utils.h"
#include "wte/WtEditorContext.h"


#define TD_TRACE_TAG "WtEditor"

#define WORKSPACE_FILE_EXTENSION	"wtw"
#define RESOURCE_FILE_EXTENSION		"wtr"
#define SCENE_FILE_EXTENSION		"wts"

using namespace std;

WtEditor::WtEditor(QWidget *parent, Qt::WFlags flags, int argc, char* argv[])
	: QMainWindow(parent, flags){

	if(argc >= 2){
		mCmdArg = argv[1];
	}
	
	// Setup the editor context
	ui.setupUi(this);

	td_setCallbackFnc(logCallback, this);

	FILE* logFile = fopen("wt-editor.log", "wb");
	td_setFileOutput(logFile);

	// World edit tab
	mWorldEdit = new WorldEditTab(this);
	ui.mainTabWidget->addTab(mWorldEdit, "World edit");
	mWorldEdit->ui.sceneView->setRenderer(WTE_CTX.getRenderer());

	connect(mWorldEdit->ui.sceneView, SIGNAL(initialized()),
		this, SLOT(onOpenGLContextCreated()));

	// Model importer tab
	ModelImporterTab* importer = new ModelImporterTab(this, WTE_CTX.getAssets());
	ui.mainTabWidget->addTab(importer, "Model importer");
	
	ui.mainTabWidget->setCurrentWidget(mWorldEdit);

	try{
		WTE_CTX.getScene()->getPhysics()->connectToVisualDebugger(
					"127.0.0.1", // address
					5425, // port
					100		// timeout
		);
	}
	catch(...){
	}

	connect(&WTE_CTX, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));

	connect(&WTE_CTX, SIGNAL(assetsUnloaded()),
		this, SLOT(onAssetsUnloaded()));
}

void WtEditor::onWorkspaceSave(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save workspace", WTE_CTX.getWorkspaceFilePath(), "Workspace files (*." WORKSPACE_FILE_EXTENSION ")");

	if(!path.size()){
		return;
	}

#if 0
	wt::lua::State state;

	wt::lua::LuaObject workspace;

	if( !(workspace = WTE_CTX.getLuaState()->getGlobals().Get("workspace")).IsTable() ){
		// Create workspace table
		WTE_CTX.getLuaState()->assignTable(workspace);
		WTE_CTX.getLuaState()->getGlobals().Set("workspace", workspace);
	}

	wt::String root = WTE_CTX.getWorkspaceFilePath().toStdString();
	wt::String assets=WTE_CTX.getAssetsFilePath().toStdString(), scene=WTE_CTX.getSceneFilePath().toStdString();
	wt::utils::replacePathSplitters(root, '/');
	wt::utils::replacePathSplitters(assets, '/');
	wt::utils::replacePathSplitters(scene, '/');

	workspace.Set("root", root.c_str());
	workspace.Set("assets", assets.c_str());
	workspace.Set("scene", scene.c_str());

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	stream.print("WORKSPACE=");
	wt::lua::serializeTable(workspace, stream);

	LOGI("Workspace saved to \"%s\"", path.toStdString().c_str());
#else
	WTE_CTX.saveWorkspace(path);
#endif
}

void WtEditor::onOpenGLContextCreated(){
	LOG("OpenGL context created");


	showMaximized();

	if(mCmdArg.size()){
#if 0
		LOGI("Loading config from file \"%s\"", mCmdArg.toStdString().c_str());

		wt::lua::State state;
		state.getStateOwner()->DoFile(mCmdArg.toStdString().c_str());

		wt::LuaObject desc = state.getGlobals().Get("WORKSPACE");

		wt::String root, assets, scene;

		if(!wt::lua::luaConv(desc.Get("root"), root)
			|| !wt::lua::luaConv(desc.Get("assets"), assets)
			|| !wt::lua::luaConv(desc.Get("scene"), scene)){
				LOGE("Invalid wtw file \"%s\"", mCmdArg.toStdString().c_str());
				return;
		}

		LOGI("config file parsed; workspace=\"%s\"; assets=\"%s\"; scene=\"%s\"",
			root.c_str(), assets.c_str(), scene.c_str());

		WTE_CTX.switchWorkspace(root.c_str());
		WTE_CTX.loadAssets(assets.c_str());
		WTE_CTX.loadScene(scene.c_str());
#else
		LOGI("Loading config from file \"%s\"", mCmdArg.toStdString().c_str());

		WTE_CTX.loadWorkspace(mCmdArg);
#endif
	}
}

WtEditor::~WtEditor(){
}

void WtEditor::updateTitle(){
	ui.statusBar->showMessage( WTE_CTX.getWorkspaceFilePath() + " " + WTE_CTX.getAssetsFilePath() + " " + WTE_CTX.getSceneFilePath());
}

void WtEditor::onWorkspaceSwitch(){
#if 0
#if 0
	if(!askYesNo(this, "Warning: All unsaved assets/scenes are going to be lost. Switch workspace?")){
		return;
	}
#endif

	QString dir = QFileDialog::getExistingDirectory(this, "Browse workspace");

	if(!dir.size()){
		return;
	}

	// TODO fix this in the file system class
	dir = dir + "/";

	WTE_CTX.switchWorkspace(dir);
#else
	QString rootDir = QFileDialog::getExistingDirectory(this, "Browse workspace");

	if(!rootDir.size()){
		return;
	}

	QString path = QFileDialog::getSaveFileName(this,
		"Pick workspace file path", rootDir, "Workspace files (*." WORKSPACE_FILE_EXTENSION ")");

	if(!path.size()){
		return;
	}

	// TODO check if path is part of rootDir

	WTE_CTX.createWorkspace(rootDir, path);
#endif
}

void WtEditor::onAssetsSaveAs(){
	if(!WTE_CTX.isAssetsLoaded()){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", WTE_CTX.getWorkspaceRootDir(), "Asset files (*." RESOURCE_FILE_EXTENSION ")");

	if(!path.size()){
		return;
	}

	try{
		WTE_CTX.saveAssets(path);
	}catch(...){
		TRACEE("Error saving assets");
		return;
	}
}

void WtEditor::onAssetsSave(){
	if(!WTE_CTX.isAssetsLoaded()){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(QMessageBox::question(this, "Confirmation",
		"Save current assets to \"" + WTE_CTX.getAssetsFilePath() + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
		WTE_CTX.saveAssets( WTE_CTX.getAssetsFilePath() );
	}
}

void WtEditor::onAssetsNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", WTE_CTX.getWorkspaceRootDir(), "Asset files (*." RESOURCE_FILE_EXTENSION ")");

	if(!path.size()){
		return;
	}


	WTE_CTX.createNewAssets(path);
}

void WtEditor::onAssetsOpen(){
#if 0
	QString path = QFileDialog::getOpenFileName(this,
		"Load assets", mWorkspacePath, "Asset files (*." RESOURCE_FILE_EXTENSION ")");
#else
	QString path = FilePicker::getFile(this, WTE_CTX.getWorkspaceRootDir(), "*." RESOURCE_FILE_EXTENSION);
#endif

	if(!path.size()){
		return;
	}

	// Unload existing assets & scene
	WTE_CTX.unloadAssets();

	// Load new assets
	WTE_CTX.loadAssets(path);
}

void WtEditor::onSceneReload(){
	if(!askYesNo(this, "Reload entire scene?\n All unsaved changes are going to be lost.")){
		return;
	}

	LOGD("Reloading scene");

	QString scenePath = WTE_CTX.getSceneFilePath();
	WTE_CTX.unloadScene();
	WTE_CTX.loadScene(scenePath);

	LOGD("Scene reloaded OK");
}


void WtEditor::onSceneClear(){
	if(!askYesNo(this, "Clear entire scene?")){
		return;
	}

	// TODO callbacks

	WTE_CTX.getScene()->clear();
}

void WtEditor::onAssetsClear(){
	if(!askYesNo(this, "Clear all assets?\n All unsaved changes are going to be lost (including current scene)")){
		return;
	}

	// TODO callbacks

	//// Destroy all resource tab items & unload all resources
	//for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
	//	(*i)->destroyAll();
	//}
	TRACEW("Missing destroyAll call?");
}

void WtEditor::onAssetsReload(){
	if(!askYesNo(this, "Reload entire assets & scene?\n All unsaved changes are going to be lost.")){
		return;
	}

	WTE_CTX.reloadAssets();
}

void WtEditor::onSceneOpen(){
	if(!WTE_CTX.isAssetsLoaded()){
		TRACEE("Assets must first be loaded");
		return;
	}

	QString path = FilePicker::getFile(this, WTE_CTX.getWorkspaceRootDir(), "*." SCENE_FILE_EXTENSION);

	if(!path.size()){
		return;
	}
	
	WTE_CTX.loadScene(path);
}

void WtEditor::onSceneSaveAs(){
	if(!WTE_CTX.isAssetsLoaded()){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(!WTE_CTX.isSceneLoaded()){
		TRACEE("Scene file must first be loaded / created");
		return;
	}

	QString path = QFileDialog::getSaveFileName(this,
		"Save scene", WTE_CTX.getWorkspaceRootDir(), "Scene files (*." SCENE_FILE_EXTENSION ")");

	if(!path.size()){
		return;
	}

	try{
		WTE_CTX.saveScene(path);
	}catch(...){
		TRACEE("Error saving scene");
	}
}

void WtEditor::onSceneSave(){
	if(!WTE_CTX.isAssetsLoaded()){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(!WTE_CTX.isSceneLoaded()){
		TRACEE("Scene file must first be loaded / created");
		return;
	}

	if(QMessageBox::question(this, "Confirmation",
		"Save current scene to \"" + WTE_CTX.getSceneFilePath() + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
		WTE_CTX.saveScene(WTE_CTX.getSceneFilePath());
	}
}

void WtEditor::onSceneNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", WTE_CTX.getWorkspaceRootDir(), "Scene files (*." SCENE_FILE_EXTENSION ")");

	WTE_CTX.createNewScene(path);
}

void WtEditor::logCallback(void* opaque, const tdchar* tag, enum TdTraceLevel level, const tdchar* message){
	WtEditor* thiz = static_cast<WtEditor*>(opaque);

	QListWidgetItem* item = new QListWidgetItem(thiz->ui.logList);

	item->setText(QString(tag) + " : " + message);

	QColor color;

	switch(level){
	case eTD_LEVEL_VERBOSE:
		color = QColor(127, 127, 127);
		break;
	case eTD_LEVEL_DEBUG:
		color = QColor(255, 255, 255);
		break;
	case eTD_LEVEL_INFO:
		color = QColor(0, 255, 0);
		break;
	case eTD_LEVEL_WARNING:
		color = QColor(255, 100, 100);
		break;
	case eTD_LEVEL_ERROR:
		color = QColor(255, 0, 0);
		break;
	}

	item->setBackgroundColor(QColor(0, 0, 0));
	item->setTextColor(color);

	thiz->ui.logList->addItem(item);

	thiz->ui.logList->scrollToItem(item);
	thiz->ui.logList->setCurrentItem(item);

	if(thiz->ui.logList->count() > 10){
		thiz->ui.logList->removeItemWidget( thiz->ui.logList->item(0) );
		//delete thiz->ui.logList->takeItem(0);
	}
}

void WtEditor::onAssetsUnloaded(){
}

void WtEditor::onAssetsLoaded(){
}
