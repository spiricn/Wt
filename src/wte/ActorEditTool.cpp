#include "stdafx.h"

#include <qmessagebox.h>

#include <wt/AGameInput.h>

#include "wte/ActorCreationDialog.h"
#include "wte/ActorEditTool.h"
#include "wte/DoodadEditDialog.h"

#define TD_TRACE_TAG "ActorEditTool"

ActorEditTool::ActorEditTool(SceneView* sceneView, QWidget* parent, AToolManager* toolManager, wt::AResourceSystem* assets)
	: QDialog(parent),  ATool(toolManager), mAssets(assets), mSelectedActor(NULL){
    ui.setupUi(this);

	mSceneView = sceneView;

	/*connect(mSceneView, SIGNAL(initialized()),
		this, SLOT(onSceneInitialized()));

	connect(mSceneView, SIGNAL(onMouseDrag(MouseDragEvent)),
		this, SLOT(onMouseDrag(MouseDragEvent)));*/

	connect(mSceneView, SIGNAL(onMouseDown(QMouseEvent*)),
		 this, SLOT(onMousePress(QMouseEvent*)));
	 

	mScene = sceneView->getScene();
	mPhysics = mScene->getPhysics();
}

void ActorEditTool::onScaleChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransform().setScale(
			ui.transform->getScale()
			);
	}
}


void ActorEditTool::onTranslationChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransform().setPosition(
			ui.transform->getTranslation()
		);
	}
}

void ActorEditTool::onRotationChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransform().setRotation(
			ui.transform->getRotation()
			);
	}
}

void ActorEditTool::onMousePress(QMouseEvent* evt){
	if(!isToolFocused()){
		return;
	}

	if((evt->button() == Qt::LeftButton && mSelectingActor) || evt->button() == Qt::MiddleButton ){
		wt::RaycastHitEvent res;
		
		if(mPhysics->pick(mScene->getCamera(), mSceneView->getRenderer().getFrustum(), glm::vec2(evt->x(), evt->y()), glm::vec2(mSceneView->width(), mSceneView->height()), res, 0, true)){
			selectActor(res.mPickedActor->getSceneActor());
		}
	}
}


void ActorEditTool::onSelectActor(){
	if(!isToolFocused()){
		return;
	}

	mSelectingActor = true;
}

//void ActorEditTool::onSave(){
//	/*QString path = QFileDialog::getSaveFileName(this,
//		"Save", "world.lua");
//
//	if(path.size()){
//		mGameLevel->serialize(path.toStdString());
//	}*/
//}
//
//void ActorEditTool::onAddDoodad(){
//	DoodadEditDialog::EditResult res = DoodadEditDialog::edit(this);
//	if(res.ok){
//#if 0
//		// TODO
//		addActor(mGameLevel->spawnDoodad(
//			"", res.model->getPath(), res.skin.toStdString(), TheGame.scene->getCamera().getPosition()
//			));
//#endif
//	}
//}
//
void ActorEditTool::updateSelectionStats(){
	if(!mSelectedActor){
		return;
	}

	glm::vec4 rot;
	mSelectedActor->getTransform().getRotation(rot);

	ui.transform->setRotation(rot.swizzle(glm::X, glm::Y, glm::Z), rot.w);

	ui.transform->setTranslation( mSelectedActor->getTransform().getPosition() );

	ui.transform->setScale( mSelectedActor->getTransform().getScale() );
}

//
//void ActorEditTool::onMouseDrag(MouseDragEvent evt){
//	if(!isToolFocused()){
//		return;
//	}
//
//
//	//bool statsChanged = false;
//
//	//if(evt.button == Qt::LeftButton){
//	//	if(mSelectedActor){
//
//	//		float smoothFactor = 0.3;
//
//	//		bool shiftDown = wt::AGameInput::isKeyDown(VK_LSHIFT);
//	//		if(wt::AGameInput::isKeyDown('X')){
//	//			if(shiftDown){
//	//				mSelectedActor->rotate(
//	//					1.0, 0.0, 0.0, evt.dx * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//			else{
//	//				mSelectedActor->move(
//	//					glm::vec3(1.0, 0.0, 0.0) * evt.dx * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//		}
//	//		else if(wt::AGameInput::isKeyDown('Y')){
//	//			if(shiftDown){
//	//				mSelectedActor->rotate(
//	//					0.0, 1.0, 0.0, evt.dx * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//			else{
//	//				mSelectedActor->move(
//	//					glm::vec3(0.0, 1.0, 0.0) * evt.dy * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//		}
//	//		else if(wt::AGameInput::isKeyDown('Z')){
//	//			if(shiftDown){
//	//				mSelectedActor->rotate(
//	//					0.0, 0.0, 1.0, evt.dx * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//			else{
//	//				mSelectedActor->move(
//	//					glm::vec3(0.0, 0.0, 1.0) * evt.dy * smoothFactor
//	//					);
//	//				statsChanged  = true;
//	//			}
//	//		}
//	//		else{
//	//			wt::RaycastHitEvent res;
//
//	//			if(mPhysics->pick(mScene->getCamera(), mSceneView->getRenderer().getFrustum(),
//	//				glm::vec2(evt.x, evt.y), glm::vec2(mSceneView->width(), mSceneView->height()), res)){
//	//					// TODO handle this better 
//	//					if(mSelectedActor != res.mPickedActor->getSceneActor()->getUserData()){
//	//						// condition checks if the hit actor isn't the selected actor (prevents the user from moving the actor to a point that's on itself)
//	//						mSelectedActor->setPosition(
//	//							res.mImpact);
//	//						statsChanged  = true;
//	//					}
//	//			}
//	//		}
//	//	}
//	//}
//	//if(statsChanged){
//	//	updateSelectionStats();
//	//}
//}
//
//void ActorEditTool::onSceneInitialized(){
//	 //for(wt::EntityManager::EntityMap::iterator i=mGameLevel->getEntityManager()->getActors().begin(); i!=mGameLevel->getEntityManager()->getActors().end(); i++){
//		// addActor(i->second);
//	 //}
//}
//
//void ActorEditTool::onAnimationSelected(QString s){
//	//if(mSelectedActor && mSelectedActor->hasAnimation()){
//	//	mSelectedActor->getAnimationPlayer()->play(s.toStdString(), true);
//	//}
//}
//
void ActorEditTool::onToggleBrush(){
	if(isToolFocused()){
		getToolManager()->giveupFocus(this);
	}
	else{
		getToolManager()->requestFocus(this);
	}
}

void ActorEditTool::selectActor(wt::ASceneActor* actor){
	mSelectedActor = actor;

	wt::Scene& scene = *mScene;

	glm::vec3 dir = glm::normalize(actor->getTransform().getPosition() - mScene->getCamera().getPosition());
	float distance = glm::length(actor->getTransform().getPosition() - mScene->getCamera().getPosition());
	const float d=50.0f;
	if(distance > d){
		mScene->getCamera().setPosition(
			actor->getTransform().getPosition() - dir*d);
	}
	mScene->getCamera().lookAt(actor->getTransform().getPosition());

	ui.comboBoxAnimation->clear();

	// TODO
	/*for(wt::Model::AnimationMap::iterator i=actor->getModel()->getAnimations().begin(); i!=actor->getModel()->getAnimations().end(); i++){
		ui.comboBoxAnimation->addItem(i->first.c_str());
	}*/

	updateSelectionStats();
}


void ActorEditTool::onNewActor(){
	ActorCreationDialog::EditResult res = ActorCreationDialog::edit(this, mAssets);

	if(res.ok){
		wt::ModelledActor* sceneActor;

		// Create scene actor
		sceneActor = mScene->createModelledActor(res.name.toStdString());

		sceneActor->setModel(res.model, res.skin->getName());

		sceneActor->getTransform().setPosition(
			mScene->getCamera().getPosition() + mScene->getCamera().getForwardVec()*10.0f);

		// Create physics actor
		if(res.physicsDesc.geometryType != wt::PhysicsActor::eGEO_TYPE_NONE){
			// Initial transform
			res.physicsDesc.pose.setPosition( sceneActor->getTransform().getPosition() );
			try{
				mPhysics->createActor(sceneActor, res.physicsDesc);
			}catch(wt::Exception& e){
				QMessageBox::critical(this, "Error", 
					QString("Error creating physics actor\n\n") + e.getDescription().c_str());
			}
		}

		mPhysics->createBBox(sceneActor);

		LOGI("Actor created");

	//	addActor(doodad);
	}
}