#include "stdafx.h"

#include <wt/Utils.h>

#include "wte/ModelImporterTab.h"

#include "wte/WtEditor.h"

using namespace std;

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
		color = QColor(0, 255, 255);
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

WtEditor::WtEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), mWorkspaceFilePath("world.lua"), mScene(new wt::Physics(new wt::EventManager), &mAssets), mAssets(wt::Assets::eFS_DIR, "D:\\Documents\\prog\\c++\\workspace\\Wt\\workspace\\"){
	ui.setupUi(this);

	//mGameLevel = new wt::GameLevel(&mAssets, &mScene, &mEntityManager);

	td_setCallbackFnc(logCallback, this);

	// World edit tab
	mWorldEdit = new WorldEditTab(this, &mScene, &mAssets);
	ui.mainTabWidget->addTab(mWorldEdit, "World edit");

	connect(mWorldEdit, SIGNAL(initialized()),
		this, SLOT(onInitialized()));

	connect(mWorldEdit, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));

	// Model importer tab
	ModelImporterTab* importer = new ModelImporterTab(this, &mAssets);
	ui.mainTabWidget->addTab(importer, "Model importer");
	
	ui.mainTabWidget->setCurrentWidget(mWorldEdit);
	//ui.mainTabWidget->setCurrentWidget(importer);

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

}

void WtEditor::onInitialized(){
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

	mWorldEdit->loadLevel(path);
}


void WtEditor::addTab(ARsrcManagerTab* tab, const QString& name){
	ui.managersTabWidget->addTab(tab, name);
	mTabs.push_back(tab);
}

void WtEditor::onAssetsLoaded(){
		
	for(TabList::iterator i=mTabs.begin(); i!=mTabs.end(); i++){
		(*i)->refreshAll();
	}


	showMaximized();
}

void WtEditor::onReload(){
	mAssets.reload();
}

void WtEditor::onBtnSaveClick(){
	if(QMessageBox::question(this, "Confirmation",
		"Save current level to \"" + mWorkspaceFilePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){

		mAssets.serialize( mWorkspaceFilePath.toStdString() );
	}
	
}

void WtEditor::onNew(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save", "", "Level files (*.lua)");
	if(!path.size()){
		return;
	}

	unloadLevel();

	mWorkspaceFilePath = path;
}


void WtEditor::onOpen(){
	QString path = QFileDialog::getOpenFileName(this,
		"Load level", "", "Level files (*.lua)");
	if(!path.size()){
		return;
	}

	unloadLevel();
	loadLevel(path);
}

