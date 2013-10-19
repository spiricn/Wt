#include "stdafx.h"

#include <wt/Utils.h>
#include <wt/SceneLoader.h>

#include "wte/ModelImporterTab.h"
#include "wte/ParticleManagerTab.h"
#include "wte/WtEditor.h"
#include "wte/Utils.h"

#define TD_TRACE_TAG "WtEditor"

using namespace std;

WtEditor::WtEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), mAssetsFilePath(""), mSceneFilePath(""), mSceneLoaded(false), mAssetsLoaded(false),
	mEventManager(&mLuaState), mScene(new wt::Physics(&mEventManager), &mAssets, &mEventManager, &mLuaState),
	mAssets(wt::Assets::eFS_DIR, "D:\\Documents\\prog\\c++\\workspace\\Wt\\workspace\\"), mRenderer(&mEventManager){

	ui.setupUi(this);

	td_setCallbackFnc(logCallback, this);

	// World edit tab
	mWorldEdit = new WorldEditTab(this, &mScene, &mAssets, &mEventManager);
	ui.mainTabWidget->addTab(mWorldEdit, "World edit");
	mWorldEdit->ui.sceneView->setRenderer(&mRenderer);

	connect(mWorldEdit->ui.sceneView, SIGNAL(initialized()),
		this, SLOT(onOpenGLContextCreated()));

	// Model importer tab
	ModelImporterTab* importer = new ModelImporterTab(this, &mAssets);
	ui.mainTabWidget->addTab(importer, "Model importer");
	
	ui.mainTabWidget->setCurrentWidget(mWorldEdit);

	/* MANAGERS TAB */
	/* Image */
	addTab(new ImageManagerTab(this, &mAssets), "Images");

	/* Texture */
	addTab(new TextureManagerTab(this, &mAssets), "Textures");

	/* Model */
	addTab(new ModelManagerTab(this, &mAssets), "Models");

	/* Animation */
	addTab(new AnimationManagerTab(this, &mAssets), "Animations");

	/* SkyBox */
	addTab(new SkyBoxManagerTab(this, &mAssets), "Sky boxes");

	/* Sound */
	addTab(new SoundManagerTab(this, &mAssets), "Sound effects");

	addTab(new ParticleManagerTab(this, &mAssets), "Particle manager");

	try{
		mScene.getPhysics()->connectToVisualDebugger(
					"127.0.0.1", // address
					5425, // port
					100		// timeout
		);
	}
	catch(...){
	}
}

void WtEditor::onOpenGLContextCreated(){
	LOG("OpenGL context created");

	showMaximized();
}

WtEditor::~WtEditor(){
}

void WtEditor::loadScene(const QString& path){
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
		return;
	}

	mWorldEdit->onSceneLoaded();

	mSceneLoaded = true;

	LOGD("Scene loaded OK");
}

void WtEditor::unloadScene(){
	if(!mSceneLoaded){
		return;
	}

	LOGD("Unloading scene ..");

	mWorldEdit->onBeforeSceneUnload();

	mSceneFilePath = "";

	mScene.clear();
	
	mSceneLoaded = false;

	LOGD("Scene unloaded OK");
}

void WtEditor::loadAssets(const QString& path){
	LOGD("Loading resources from file \"%s\"", path.toStdString().c_str());

	// Unload existing resources
	unloadAssets();

	mAssetsFilePath = path;
	setWindowTitle(path);

	//ui.sceneView->makeCurrent();

	LuaPlus::LuaStateOwner state;
	state->DoFile(path.toStdString().c_str());

	try{
		mAssets.load( state->GetGlobal("ASSETS") );
	}catch(...){
		QMessageBox::critical(this, "Error", "Error loading assets from \"" + path + "\"");
		return;
	}

	mAssetsLoaded = true;

	LOGD("Resources loaded OK");
}

void WtEditor::unloadAssets(){
	if(!mAssetsLoaded){
		return;
	}

	LOGD("Unloading resources ..");

	mWorldEdit->onBeforeAssetsUnload();

	// Unload existing scene
	unloadScene();

	// Destroy all resource tab items & unload all resources
	for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
		(*i)->destroyAll();
	}

	mAssetsFilePath = "";

	mAssetsLoaded = false;

	LOGD("Resource unloaded OK");
}

void WtEditor::addTab(ARsrcManagerTab* tab, const QString& name){
	ui.managersTabWidget->addTab(tab, name);
	mTabs.push_back(tab);
}

void WtEditor::onAssetsSaveAs(){
	if(!mAssetsLoaded){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", "", "Asset files (*.lua)");

	if(!path.size()){
		return;
	}

	try{
		saveAssets(path);
		mAssetsFilePath = path;
	}catch(...){
		TRACEE("Error saving assets");
		return;
	}
}

void WtEditor::onAssetsSave(){
	if(!mAssetsLoaded){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(QMessageBox::question(this, "Confirmation",
		"Save current assets to \"" + mAssetsFilePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
		saveAssets( mAssetsFilePath );
	}
}

void WtEditor::onAssetsNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", "", "Asset files (*.lua)");

	if(!path.size()){
		return;
	}

	// Unload existing assets & scene
	unloadAssets();

	try{
		// Create an empty assets file
		saveAssets( path );

		mAssetsFilePath = path;

		mAssetsLoaded = true;
	}catch(...){
		TRACEE("Error creating new assets file");
		return;
	}
}

void WtEditor::onAssetsOpen(){
	QString path = QFileDialog::getOpenFileName(this,
		"Load assets", "", "Asset files (*.lua)");

	if(!path.size()){
		return;
	}

	// Unload existing assets & scene
	unloadAssets();

	// Load new assets
	loadAssets(path);
}

void WtEditor::onSceneReload(){
	if(!askYesNo(this, "Reload entire scene?\n All unsaved changes are going to be lost.")){
		return;
	}

	LOGD("Reloading scene");

	QString scenePath = mSceneFilePath;
	unloadScene();
	loadScene(scenePath);

	LOGD("Scene reloaded OK");
}


void WtEditor::onSceneClear(){
	if(!askYesNo(this, "Clear entire scene?")){
		return;
	}

	// TODO callbacks

	mScene.clear();
}

void WtEditor::onAssetsClear(){
	if(!askYesNo(this, "Clear all assets?\n All unsaved changes are going to be lost (including current scene)")){
		return;
	}

	// TODO callbacks

	// Destroy all resource tab items & unload all resources
	for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
		(*i)->destroyAll();
	}
}

void WtEditor::onAssetsReload(){
	if(!askYesNo(this, "Reload entire assets & scene?\n All unsaved changes are going to be lost.")){
		return;
	}

	LOGD("Reloading everything ...");

	QString scenePath = mSceneFilePath;
	QString assetsPath = mAssetsFilePath;

	// Unload resources / scene
	unloadAssets();

	loadAssets(assetsPath);
	loadScene(scenePath);

	LOGD("Everything reloaded OK");
}

void WtEditor::onSceneOpen(){
	if(!mAssetsLoaded){
		TRACEE("Assets must first be loaded");
		return;
	}

	QString path = QFileDialog::getOpenFileName(this,
		"Load scene", "", "Scene files (*.lua)");

	if(!path.size()){
		return;
	}
	
	loadScene(path);
}

void WtEditor::onSceneSaveAs(){
	if(!mAssetsLoaded){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(!mSceneLoaded){
		TRACEE("Scene file must first be loaded / created");
		return;
	}

	QString path = QFileDialog::getSaveFileName(this,
		"Save scene", "", "Scene files (*.lua)");

	if(!path.size()){
		return;
	}

	try{
		saveScene(path);
		mSceneFilePath = path;
	}catch(...){
		TRACEE("Error saving scene");
	}
}

void WtEditor::onSceneSave(){
	if(!mAssetsLoaded){
		TRACEE("Assets file must first be loaded / created");
		return;
	}

	if(!mSceneLoaded){
		TRACEE("Scene file must first be loaded / created");
		return;
	}

	if(QMessageBox::question(this, "Confirmation",
		"Save current scene to \"" + mSceneFilePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
		saveScene(mSceneFilePath);
	}
}

void WtEditor::onSceneNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save assets", "", "Asset files (*.lua)");

	if(!path.size()){
		return;
	}

	// Unload existing scene
	unloadScene();

	try{
		saveScene(path);
		mSceneFilePath = path;
		mSceneLoaded = true;
	}catch(...){
		TRACEE("Error creating new assets file");
		return;
	}
}

void WtEditor::saveScene(const QString& path){
	wt::SceneLoader loader(&mScene, &mAssets);

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	loader.save(stream);
}

void WtEditor::saveAssets(const QString& path){
	mAssets.serialize(path.toStdString());
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
}