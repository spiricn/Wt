#include "stdafx.h"

#include <wt/Utils.h>

#include "wte/ModelImporterTab.h"
#include "wte/ParticleManagerTab.h"
#include "wte/WtEditor.h"


#define TD_TRACE_TAG "WtEditor"

using namespace std;

WtEditor::WtEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), mWorkspaceFilePath("world.lua"),
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

	connect(mWorldEdit, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));

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

	loadLevel("workspace/level1.lua");
	loadScene("workspace/scene.lua");

	showMaximized();
}


WtEditor::~WtEditor(){
}

void WtEditor::unloadLevel(){
	mWorldEdit->unloadLevel();

	for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
		(*i)->destroyAll();
	}
}

void WtEditor::loadLevel(const QString& path){
	mWorkspaceFilePath = path;
	setWindowTitle(path);

	mWorldEdit->loadResources(path);
}


void WtEditor::addTab(ARsrcManagerTab* tab, const QString& name){
	ui.managersTabWidget->addTab(tab, name);
	mTabs.push_back(tab);
}

void WtEditor::onAssetsLoaded(){	
	for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
		(*i)->refreshAll();
	}
}

void WtEditor::onReload(){
	mAssets.reload();
	mWorldEdit->loadScene(mScenePath);
}

void WtEditor::onSaveSceneAs(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save scene", "", "Scene files (*.lua)");
	if(!path.size()){
		return;
	}


	mScenePath = path;
	mWorldEdit->saveScene(mScenePath);
}

void WtEditor::onBtnSaveClick(){
	if(QMessageBox::question(this, "Confirmation",
		"Save current level to \"" + mWorkspaceFilePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){

		mAssets.serialize( mWorkspaceFilePath.toStdString() );
	}
	
}

void WtEditor::onNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save resources", "", "Resource files (*.lua)");
	if(!path.size()){
		return;
	}

	unloadLevel();

	mWorkspaceFilePath = path;
}

void WtEditor::onOpen(){
	QString path = QFileDialog::getOpenFileName(this,
		"Load resources", "", "Resource files (*.lua)");
	if(!path.size()){
		return;
	}

	unloadLevel();
	loadLevel(path);
}

void WtEditor::loadScene(const QString& path){
	mScenePath = path;
	mWorldEdit->loadScene(path);
}

void WtEditor::onLoadScene(){
	QString path = QFileDialog::getOpenFileName(this,
		"Load scene", "", "Scene files (*.lua)");
	if(!path.size()){
		return;
	}
	
	loadScene(path);
}

void WtEditor::onSaveScene(){
	if(QMessageBox::question(this, "Confirmation",
		"Save current scene to \"" + mScenePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){

		mWorldEdit->saveScene(mScenePath);
	}
}

void WtEditor::logCallback(void* opaque, const tdchar* tag, enum TdTraceLevel level, const tdchar* message){
	WtEditor* thiz = static_cast<WtEditor*>(opaque);

	QListWidgetItem* item = new QListWidgetItem(thiz->ui.logList);

	item->setText(QString(tag) + " : " + message);

	QColor color;

	switch(level){
	case eTD_LVL_VERBOSE:
		color = QColor(127, 127, 127);
		break;
	case eTD_LVL_DEBUG:
		color = QColor(255, 255, 255);
		break;
	case eTD_LVL_INFO:
		color = QColor(0, 255, 0);
		break;
	case eTD_LVL_WARNING:
		color = QColor(255, 100, 100);
		break;
	case eTD_LVL_ERROR:
		color = QColor(255, 0, 0);
		break;
	}

	item->setBackgroundColor(QColor(0, 0, 0));
	item->setTextColor(color);

	thiz->ui.logList->addItem(item);

	
	thiz->ui.logList->scrollToItem(item);
	thiz->ui.logList->setCurrentItem(item);
}

// TODO move these to WorldEdit

void WtEditor::onCreateTerrain(){
	mWorldEdit->onCreateTerrain();
}

void WtEditor::onClearScene(){
	mWorldEdit->onClearAll();
}

void WtEditor::onSetSkybox(){
	mWorldEdit->onSetSkybox();
}

void WtEditor::onScreenshot(){
	mWorldEdit->onScreenshot();
}
