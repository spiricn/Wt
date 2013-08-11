#include "stdafx.h"

#include <qcheckbox.h>
#include <QColorDialog>
#include <qtoolbar.h>
#include <qprocess.h>
#include <qmessagebox.h>

#include "wte/MainGLWidget.h"
#include "wte/WorldEditTab.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/TerrainEditTool.h"
#include "wte/LightEditTool.h"
#include "wte/ActorEditTool.h"
#include "wte/FogTool.h"
#include "wte/TerrainEditDialog.h"
#include "wte/GodrayTool.h"

#include <wt/SFSound.h>
#include <wt/SceneLoader.h>

#define FPS 50
	

WorldEditTab::WorldEditTab(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets, wt::EventManager* evtManager) : QMainWindow(parent),
	mSelectedActor(NULL), mAssets(assets), mScene(scene), mEventManager(evtManager){
	 ui.setupUi(this);

	 // Scene setup
	 ui.sceneView->setScene(mScene);

	// actor tool
	 mActorEditTool = new ActorEditTool(ui.sceneView, this, this, mAssets);
	 ui.actorEditToolDock->setWidget(mActorEditTool);
	 mTools.push_back(mActorEditTool);

	 //ui.actorEditToolDock->setVisible(false);

	 // Terrain tool
	 mTerrainEditTool = new TerrainEditTool(ui.sceneView, this, this, mScene, mAssets);
	 mTerrainEditTool->setModal(false);
	 ui.terrainToolDock->setWidget(mTerrainEditTool);
	 mTools.push_back(mTerrainEditTool);
	 ui.terrainToolDock->setVisible(false);

	 //// light tool
	 mLightTool = new LightEditTool(ui.sceneView, this);
	 mLightTool->setModal(false);
	 ui.lightToolDock->setWidget(mLightTool);
	 ui.lightToolDock->setVisible(false);


	 GodrayTool* godTool = new GodrayTool(this, mScene, mAssets);
	 ui.godrayToolDock->setWidget(godTool);	 
	
#if 0
	 // fog tool
	 mFogTool = new FogTool(ui.sceneView, this);
	 ui.fogToolDock->setWidget(mFogTool);
#endif
	 ui.fogToolDock->setVisible(false);

	 // main loop timer
	 connect(&mTimer, SIGNAL(timeout()),
		 this, SLOT(onTimeout()));

	 mTimer.start( (1.0/FPS)*1000 );

	 createToolbar();
}

WorldEditTab::~WorldEditTab(){
	// @note Not sure if this is necessary but it seems to
	// prevent the OpenGL framebuffer error on its destruction
	// (the context is probably deleted before the object itself)

	delete mTerrainEditTool;
}

void WorldEditTab::onToggleToolTerrain(){
	ui.terrainToolDock->setVisible( !ui.terrainToolDock->isVisible() );
}

void WorldEditTab::onToggleToolLight(){
	ui.lightToolDock->setVisible( !ui.lightToolDock->isVisible() );
}

void WorldEditTab::onToggleToolActor(){
	ui.actorEditToolDock->setVisible( !ui.actorEditToolDock->isVisible() );
}

void WorldEditTab::onTimeout(){
	float dt = 1.0f/FPS;

	mScene->update(dt);
	mScene->getPhysics()->update(dt);

	ui.sceneView->update(dt);

	mEventManager->tick();
}

void WorldEditTab::createToolbar(){
#if 0
	QAction* terrainEdit = new QAction(QIcon(":/icons/placeholder"), "Terrain editor", this);
	connect(terrainEdit, SIGNAL(triggered(bool)),
		this, SLOT(onShowTerrainTool(bool)));
	ui.toolBar->addAction(terrainEdit);

	QAction* lightEdit = new QAction(QIcon(":/icons/placeholder"), "Light tool", this);
	connect(lightEdit, SIGNAL(triggered(bool)),
		this, SLOT(onShowLightEditor(bool)));
	ui.toolBar->addAction(lightEdit);
#endif
}

void WorldEditTab::onShowLightEditor(bool){
	mLightTool->setVisible( !mLightTool->isVisible() );
}

void WorldEditTab::onShowTerrainTool(bool){
	mTerrainEditTool->setVisible( !mTerrainEditTool->isVisible() );
}

void WorldEditTab::onMouseDown(QMouseEvent* e){
}

void WorldEditTab::onScreenshot(){
	wt::String name = "Screenshot-" + wt::utils::getCurrentTime() + ".bmp";
	QString path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("/screenshots/") + QString(name.c_str()));

	try{
		/*ui.sceneView->getRenderer()->saveScreenshot(path.toStdString());*/
		wt::Image img;
		wt::gl::FrameBuffer::unbindRead();
		img.fromFrameBuffer(wt::Image::RGB);

		mAssets->getImageManager()->save(path.toStdString(), &img);

		LOGI("WorldEditTab", "Screenshot saved to \"%s\"", path.toStdString().c_str());
	}catch(wt::Exception& e){
		LOGE("WorldEditTab",
			"Error saving screenshot \"%s\"", e.getFullDescription().c_str());
	}

	/*QProcess* p = new QProcess();
	p->execute(path);*/
}


void WorldEditTab::saveScene(const QString& path){
	wt::SceneLoader loader(mScene, mAssets);

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	try{
		loader.save(stream);
	}catch(...){
		QMessageBox::critical(this, "Error", "Error saving scene to \"" + path + "\"");
	}
}

void WorldEditTab::loadScene(const QString& path){
	mScene->clear();

	wt::SceneLoader loader(mScene, mAssets);

	wt::FileIOStream stream(path.toStdString(), wt::AIOStream::eMODE_READ);

	try{
		loader.load(stream);
	}catch(...){
		QMessageBox::critical(this, "Error", "Error loading scene from \"" + path + "\"");
	}

	// Find terrain
	wt::Terrain* terrain = NULL;
	for(wt::Scene::ActorMap::iterator iter=mScene->getActorMap().begin(); iter!=mScene->getActorMap().end(); iter++){
		if(iter->second->getActorType() == wt::ASceneActor::eTYPE_TERRAIN){
			terrain = static_cast<wt::Terrain*>(iter->second);
			break;
		}
	}

	mTerrainEditTool->setTarget(terrain);


	emit sceneLoaded();
}

void WorldEditTab::loadResources(const QString& path){

	ui.sceneView->makeCurrent();

	LuaPlus::LuaStateOwner state;
	state->DoFile(path.toStdString().c_str());

	try{
		mAssets->load( state->GetGlobal("ASSETS") );
	}catch(...){
		QMessageBox::critical(this, "Error", "Error loading assets from \"" + path + "\"");
		return;
	}

	
	emit assetsLoaded();
	emit onTerrainCreated();
}

void WorldEditTab::onGLContextCreated(){
}

void WorldEditTab::onSetSkybox(){
	ResourcePickerDialog picker(this);

	wt::SkyBox* sky = picker.pickResource(this, mAssets->getSkyBoxManager());
	if(sky){
		mScene->setSkyBox(sky);
	}
}

void WorldEditTab::onSetTransClicked(){
}

void WorldEditTab::onGrabInputChecked(int state){
	ui.sceneView->grabInput(state);
}

void WorldEditTab::onRotSliderMoved(int d){
	if(mSelectedActor){
		float factor = d/100.0f;
		mSelectedActor->getTransform().setRotation(
			0, 1, 0, factor*360.0f);
		ui.sceneView->update();
	}
}

void WorldEditTab::onTreeItemActivated(void* data){
	//wt::SceneActor* actor = static_cast<wt::SceneActor*>(data);
	//mSelectedActor = actor;

	//const glm::vec3& pos = actor->getTransform().getPosition();
	//glm::vec4 rot;
	//wt::math::quatToAxisAngle(actor->getTransform().getQuat(), rot);

	//char bfr[256];

	//sprintf(bfr, "%f, %f, %f", pos.x, pos.y, pos.z);
	//ui.posEdit->setText(bfr);

	//sprintf(bfr, "%f, %f, %f, %f", rot.x, rot.y, rot.z, rot.w);
	//ui.rotEdit->setText(bfr);

	//ui.horizontalSlider->setValue(
	//	(rot.w/360.0)*100.0f);
}

void WorldEditTab::onClearAll(){
	unloadLevel();
}

void WorldEditTab::onSave(){
	
}

void WorldEditTab::unloadLevel(){
	mScene->clear();
}

void WorldEditTab::onCreateTerrain(){
	wt::TerrainDesc desc;
	
	
	if(!TerrainEditDialog::editTerrain(this, mAssets, desc)){
		return;
	}

	wt::Terrain* terrain = mScene->createTerrain();
	try{
		terrain->create(desc);

		mTerrainEditTool->setTarget(terrain);

		emit onTerrainCreated();
	}catch(wt::Exception& e){
		QMessageBox::critical(this, "Error", QString("Error creating terrain\n") + e.getDescription().c_str());
	}

}

void WorldEditTab::onToggleBones(){
	// TODO fix this
	static bool bones = false;
	bones = !bones;
	ui.sceneView->getRenderer()->setRenderBones(bones);
}

void WorldEditTab::onToggleBoundingBoxes(){
	// TODO fix this
	static bool boxes = false;
	boxes = !boxes;
	ui.sceneView->getRenderer()->setRenderBoundingBoxes(boxes);
}