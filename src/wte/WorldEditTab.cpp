#include "stdafx.h"

#include <qcheckbox.h>
#include <QColorDialog>
#include <qtoolbar.h>
#include <qprocess.h>
#include <qmessagebox.h>

#include <wt/SkyBox.h>
#include "wte/WorldEditTab.h"
#include "wte/TransformableAnimationDialog.h"
#include "wte/MainGLWidget.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/TerrainEditTool.h"
#include "wte/LightEditTool.h"
#include "wte/ActorEditTool.h"
#include "wte/FogTool.h"
#include "wte/TerrainEditDialog.h"
#include "wte/GodrayTool.h"
#include "wte/ProcessManagerTool.h"
#include "wte/Scripter.h"
#include "wte/ResourceTool.h"
#include "wte/ModelImporterTab.h"

#include <wt/SFSound.h>
#include <wt/SceneLoader.h>
#include <wte/WtEditorContext.h>

#define FPS ( 30 )
	

WorldEditTab::WorldEditTab(QWidget* parent) : QMainWindow(parent), mPrevDockWidget(NULL){
	ui.setupUi(this);

	// Scene setup
	ui.sceneView->setScene(WTE_CTX.getScene());

	addTool(
		"Actor",
		mActorEditTool = new ActorEditTool(ui.sceneView, this, this)
	);

	addTool(
		"Terrain",
		mTerrainEditTool = new TerrainEditTool(ui.sceneView, this, this)
	);

	addTool(
		"Light",
		mLightTool = new LightEditTool(ui.sceneView, this, this)
	);

	addTool(
		"Godray",
		mGodrayTool = new GodrayTool(this, this)
	);

	addTool(
		"Fog",
		mFogTool = new FogTool(ui.sceneView, this, this)
	);
	
	addTool(
		"Process",
		new ProcessManagerTool(this, this)
	);

	addTool(
		"Resource",
		new ResourceTool(this, this)
	);

	addTool(
		"Importer",
		new ModelImporterTab(this, this)
	);

	connect(&WTE_CTX, SIGNAL(sceneLoaded()),
		this, SLOT(onSceneLoaded()));

	connect(&WTE_CTX, SIGNAL(update(float)),
		this, SLOT(onUpdate(float)));


	// TODO move to WtEditor.cpp ?
	WTE_CTX.startLoop();
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

void WorldEditTab::onUpdate(float dt){
	WTE_CTX.getScene()->update(dt);
	WTE_CTX.getScene()->getPhysics()->update(dt);
	WTE_CTX.getProcManager()->update(dt);

	ui.sceneView->update(dt);

	WTE_CTX.getEventManager()->update();

	glm::vec3 eyePos, eyeFw;
	WTE_CTX.getScene()->getCamera().getForwardVector(eyeFw);
	WTE_CTX.getScene()->getCamera().getTranslation(eyePos);

#if 0
	// FIXME causing errors if no scene has been loaded
	mAssets->getSoundSystem()->setListenerForwardVec( eyeFw );
	mAssets->getSoundSystem()->setListenerPosition( eyePos );
#endif

	mProcManager.update(dt);
}

void WorldEditTab::onShowTerrainTool(bool){
	mTerrainEditTool->setVisible( !mTerrainEditTool->isVisible() );
}

void WorldEditTab::onClearSkybox(){
	WTE_CTX.getScene()->setSkyBox(NULL);
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

		WTE_CTX.getAssets()->getImageManager()->save(path.toStdString(), &img);

		LOGI("WorldEditTab", "Screenshot saved to \"%s\"", path.toStdString().c_str());
	}catch(wt::Exception& e){
		LOGE("WorldEditTab",
			"Error saving screenshot \"%s\"", e.getFullDescription().c_str());
	}

}

void WorldEditTab::onGLContextCreated(){
}

void WorldEditTab::onSetSkybox(){
	wt::SkyBox* sky = ResourcePickerDialog::pickResource(this, WTE_CTX.getAssets()->getSkyBoxManager());
	if(sky){
		WTE_CTX.getScene()->setSkyBox(sky);
	}
}

void WorldEditTab::onSetTransClicked(){
}


void WorldEditTab::onCameraNewAnimation(){
	TransformableAnimationDialog* dlg = new TransformableAnimationDialog(this, &WTE_CTX.getScene()->getCamera(), &mProcManager);

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

void WorldEditTab::onExecuteScript(){
#if 0
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open script file"), "", tr("Script files (*.lua)"));
#else
	QString path = "test_script.lua";
#endif

	WTE_CTX.getLuaState()->getStateOwner()->DoFile(path.toStdString().c_str());
}

void WorldEditTab::onToggleAxes(){
	// TODO fix this
	static bool state = false;
	state = !state;
	WTE_CTX.getRenderer()->setRenderAxes(state);
}