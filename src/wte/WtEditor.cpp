#include "stdafx.h"

#include <wt/Utils.h>

#include "wte/ModelImporterTab.h"

#include "wte/WtEditor.h"

using namespace std;

WtEditor::WtEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), mWorkspaceFilePath("world.lua"), mScene(new wt::Physics, &mAssets){
	ui.setupUi(this);

	mGameLevel = new wt::GameLevel(&mAssets, &mScene, &mEntityManager);
	wt::Log::setListener(this);

	mWorldEdit = new WorldEditTab(this, mGameLevel);
	ui.mainTabWidget->addTab(mWorldEdit, "World edit");

	connect(mWorldEdit, SIGNAL(initialized()),
		this, SLOT(onInitialized()));

	ModelImporterTab* importer = new ModelImporterTab(this, &mAssets);
	ui.mainTabWidget->addTab(importer, "Model importer");
	
	connect(mWorldEdit, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));

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

}

void WtEditor::onInitialized(){
	loadLevel("test.lua");
}

WtEditor::~WtEditor(){
	delete mGameLevel;
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

bool WtEditor::log(int level, const wt::String& tag, const wt::String& text){
	QListWidgetItem* item = new QListWidgetItem(ui.logList);

	item->setText((tag + " : " + text).c_str());

	QColor color;

	switch(level){
	case wt::Log::VERBOSE:
		color = QColor(127, 127, 127);
		break;
	case wt::Log::DEBUG:
		color = QColor(255, 255, 255);
		break;
	case wt::Log::INFO:
		color = QColor(0, 255, 0);
		break;
	case wt::Log::WARNING:
		color = QColor(0, 255, 255);
		break;
	case wt::Log::ERR:
		color = QColor(255, 0, 0);
		break;
	}

	item->setBackgroundColor(QColor(0, 0, 0));
	item->setTextColor(color);

	ui.logList->addItem(item);

	
	ui.logList->scrollToItem(item);
	ui.logList->setCurrentItem(item);

	return true;
}

void WtEditor::onBtnSaveClick(){
	if(QMessageBox::question(this, "Confirmation",
		"Save current level to \"" + mWorkspaceFilePath + "\" ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){

		mGameLevel->serialize( mWorkspaceFilePath.toStdString() );
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

