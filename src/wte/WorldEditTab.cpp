#include "stdafx.h"

#include <qcheckbox.h>
#include <QColorDialog>
#include <qtoolbar.h>
#include <qprocess.h>

#include "wte/WorldEditTab.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/TerrainEditTool.h"
#include "wte/LightEditTool.h"
#include "wte/ActorEditTool.h"
#include "wte/FogTool.h"
#include "wte/TerrainEditDialog.h"

#include <wt/SFSound.h>

#define FPS 50
	

WorldEditTab::WorldEditTab(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets) : QMainWindow(parent),
	mSelectedActor(NULL), mAssets(assets), mScene(scene){
	 ui.setupUi(this);

	 // Scene setup
	 connect(ui.sceneView, SIGNAL(initialized()),
		 this, SLOT(onGLContextCreated()));

	 ui.sceneView->setScene(mScene);

#if 0
	 // terrain tool
	 mTerrainEditTool = new TerrainEditTool(ui.sceneView, this, this, mScene, mAssets);
	 mTerrainEditTool->setModal(false);
	 ui.terrainToolDock->setWidget(mTerrainEditTool);
	 mTools.push_back(mTerrainEditTool);

	 //// light tool
	 mLightTool = new LightEditTool(ui.sceneView, this);
	 mLightTool->setModal(false);
	 ui.lightToolDock->setWidget(mLightTool);

	 // actor tool
	 mActorEditTool = new ActorEditTool(ui.sceneView,  mGameLevel, this, this);
	 ui.actorEditToolDock->setWidget(mActorEditTool );
	 mTools.push_back(mActorEditTool);

	 // fog tool
	 mFogTool = new FogTool(ui.sceneView, this);
	 ui.fogToolDock->setWidget(mFogTool);
#endif

	 // main loop timer
	 connect(&mTimer, SIGNAL(timeout()),
		 this, SLOT(onTimeout()));

	 mTimer.start( (1.0/FPS)*1000 );

	 createToolbar();
}

void WorldEditTab::onTimeout(){
	float dt = 1.0f/FPS;

	mScene->update(dt);
	mScene->getPhysics()->update(dt);

	ui.sceneView->update(dt);
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
	wt::String name = "Screenshot-" + wt::Utils::getCurrentTime() + ".bmp";
	QString path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("/screenshots/") + QString(name.c_str()));

	try{
		/*ui.sceneView->getRenderer().saveScreenshot(path.toStdString());*/
		wt::Image img;
		wt::Gl::FrameBuffer::unbind(wt::Gl::FrameBuffer::READ);
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

#include <qmessagebox.h>

void WorldEditTab::loadLevel(const QString& path){
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

#include <qsettings.h>

void WorldEditTab::onGLContextCreated(){
	emit initialized();
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
	//mGameLevel->clear();
	//mGameLevel->getScene()->clear();
}

void WorldEditTab::onCreateTerrain(){
	/*wt::TerrainDesc desc;
	
	if(!TerrainEditDialog::editTerrain(this, mAssets, desc)){
		return;
	}

	mScene->createTerrain(desc);

	emit onTerrainCreated();*/
}