#include "stdafx.h"

#include <qcheckbox.h>
#include <QColorDialog>
#include <qtoolbar.h>
#include <qprocess.h>
#include <qmessagebox.h>

#include "wte/CameraAnimationDialog.h"
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

#define FPS ( 30 )
	

WorldEditTab::WorldEditTab(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets, wt::EventManager* evtManager) : QMainWindow(parent),
		mAssets(assets), mScene(scene), mEventManager(evtManager){
	ui.setupUi(this);

	// Scene setup
	ui.sceneView->setScene(mScene);


	addTool(
		ui.actorEditToolDock,
		mActorEditTool = new ActorEditTool(ui.sceneView, this, this, mAssets)
	);

	addTool(
		ui.terrainToolDock,
		mTerrainEditTool = new TerrainEditTool(ui.sceneView, this, this, mScene, mAssets)
	);

	addTool(
		ui.lightToolDock,
		mLightTool = new LightEditTool(ui.sceneView, this, this)
	);

	addTool(
		ui.godrayToolDock,
		mGodrayTool = new GodrayTool(this, this, mScene, mAssets)
	);

	addTool(
		ui.fogToolDock,
		mFogTool = new FogTool(ui.sceneView, this, this)
	);

	// main loop timer
	connect(&mTimer, SIGNAL(timeout()),
		this, SLOT(onTimeout()));

	mTimer.start( (1.0/FPS)*1000 );
}

void WorldEditTab::requestFocus(ATool* tool){
	for(ToolList::Iterator i=mTools.begin(); i!=mTools.end(); i++){
		if(*i != tool){
			(*i)->onToolLostFocus();
		}
	}

	tool->onToolGainFocus();
}

void WorldEditTab::giveupFocus(ATool* tool){
	tool->onToolLostFocus();
}

WorldEditTab::~WorldEditTab(){
	// @note Not sure if this is necessary but it seems to
	// prevent the OpenGL framebuffer error on its destruction
	// (the context is probably deleted before the object itself)

	delete mTerrainEditTool;
}

void WorldEditTab::onToggleGodrayTool(){
	ui.godrayToolDock->setVisible( !ui.godrayToolDock->isVisible() );
}

void WorldEditTab::onToggleToolFog(){
	ui.fogToolDock->setVisible( !ui.fogToolDock->isVisible() );
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

	glm::vec3 eyePos, eyeFw;
	mScene->getCamera().getForwardVector(eyeFw);
	mScene->getCamera().getTranslation(eyePos);

#if 0
	// FIXME causing errors if no scene has been loaded
	mAssets->getSoundSystem()->setListenerForwardVec( eyeFw );
	mAssets->getSoundSystem()->setListenerPosition( eyePos );
#endif

	mProcManager.update(dt);
}

void WorldEditTab::onShowLightEditor(bool){
	mLightTool->setVisible( !mLightTool->isVisible() );
}

void WorldEditTab::onShowTerrainTool(bool){
	mTerrainEditTool->setVisible( !mTerrainEditTool->isVisible() );
}

void WorldEditTab::onClearSkybox(){
	mScene->setSkyBox(NULL);
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


void WorldEditTab::onCameraNewAnimation(){
	CameraAnimationDialog* dlg = new CameraAnimationDialog(this, mScene, &mProcManager);

	dlg->show();
}

void WorldEditTab::onGrabInputChecked(int state){
	ui.sceneView->grabInput(state);
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

void WorldEditTab::onAssetsLoaded(){
	// New assets have been loaded
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
	}
}

void WorldEditTab::onBeforeAssetsUnload(){
	// Handle everything before assets are unloaded
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
	}
}

void WorldEditTab::onBeforeSceneUnload(){
	// Handle everything before scnee is unloaded
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
		(*iter)->onBeforeSceneUnload();
	}
}

void WorldEditTab::onSceneLoaded(){
	// New scene has been loaded
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
		(*iter)->onSceneLoaded();
	}
}

void WorldEditTab::onAfterSceneUnload(){
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
		(*iter)->onAfterSceneUnload();
	}
}

void WorldEditTab::onAfterAssetsUnload(){
	for(ToolList::Iterator iter=mTools.begin(); iter!=mTools.end(); iter++){
		(*iter);
	}
}