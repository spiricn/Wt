#include "stdafx.h"

#include <wt/AGameInput.h>

#include "wte/ActorCreationDialog.h"
#include "wte/ActorEditTool.h"
#include "wte/DoodadEditDialog.h"

#include <wt/game/AGameActor.h>

ActorEditTool::ActorEditTool(SceneView* sceneView, wt::GameLevel* level, QWidget* parent, AToolManager* toolManager)
	: QDialog(parent), mSelectedActor(NULL), ATool(toolManager), mGameLevel(level){
    ui.setupUi(this);

	mSceneView = sceneView;

	connect(mSceneView, SIGNAL(initialized()),
		this, SLOT(onSceneInitialized()));

	connect(mSceneView, SIGNAL(onMouseDrag(MouseDragEvent)),
		this, SLOT(onMouseDrag(MouseDragEvent)));

	connect(mSceneView, SIGNAL(onMouseDown(QMouseEvent*)),
		 this, SLOT(onMousePress(QMouseEvent*)));
	 

	mScene = mGameLevel->getScene();
	mPhysics = mScene->getPhysics();
}

void ActorEditTool::onScaleChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->setScale(
			ui.transform->getScale()
			);
	}
}


void ActorEditTool::onTranslationChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->setPosition(
			ui.transform->getTranslation()
			);
	}
}

void ActorEditTool::onRotationChanged(){
	//LOG("");
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->setRotation(
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
		
		if(mPhysics->pick(mScene->getCamera(), mSceneView->getRenderer().getFrustum(),
			glm::vec2(evt->x(), evt->y()), glm::vec2(mSceneView->width(), mSceneView->height()), res)){
				wt::AGameActor* actor = (wt::AGameActor*)res.mPickedActor->getSceneActor()->getUserData();
				if(actor){
					selectActor(actor);
					mSelectingActor = false;
				}
		}
	}

}

void ActorEditTool::onSelectActor(){
	if(!isToolFocused()){
		return;
	}

	mSelectingActor = true;
}

void ActorEditTool::onSave(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save", "world.lua");

	if(path.size()){
		mGameLevel->serialize(path.toStdString());
	}
}

void ActorEditTool::onAddDoodad(){
	DoodadEditDialog::EditResult res = DoodadEditDialog::edit(this);
	if(res.ok){
#if 0
		// TODO
		addActor(mGameLevel->spawnDoodad(
			"", res.model->getPath(), res.skin.toStdString(), TheGame.scene->getCamera().getPosition()
			));
#endif
	}
}

void ActorEditTool::updateSelectionStats(){
	//LOG("");
	if(!mSelectedActor){
		return;
	}

	/*ui.transform->setRotation( mSelectedActor->getRotation() );

	ui.transform->setTranslation( mSelectedActor->getPosition() );

	ui.transform->setScale( mSelectedActor->getScale() );*/
}


void ActorEditTool::onMouseDrag(MouseDragEvent evt){
	if(!isToolFocused()){
		return;
	}


	bool statsChanged = false;

	if(evt.button == Qt::LeftButton){
		if(mSelectedActor){

			float smoothFactor = 0.3;

			bool shiftDown = wt::AGameInput::isKeyDown(VK_LSHIFT);
			if(wt::AGameInput::isKeyDown('X')){
				if(shiftDown){
					mSelectedActor->rotate(
						1.0, 0.0, 0.0, evt.dx * smoothFactor
						);
					statsChanged  = true;
				}
				else{
					mSelectedActor->move(
						glm::vec3(1.0, 0.0, 0.0) * evt.dx * smoothFactor
						);
					statsChanged  = true;
				}
			}
			else if(wt::AGameInput::isKeyDown('Y')){
				if(shiftDown){
					mSelectedActor->rotate(
						0.0, 1.0, 0.0, evt.dx * smoothFactor
						);
					statsChanged  = true;
				}
				else{
					mSelectedActor->move(
						glm::vec3(0.0, 1.0, 0.0) * evt.dy * smoothFactor
						);
					statsChanged  = true;
				}
			}
			else if(wt::AGameInput::isKeyDown('Z')){
				if(shiftDown){
					mSelectedActor->rotate(
						0.0, 0.0, 1.0, evt.dx * smoothFactor
						);
					statsChanged  = true;
				}
				else{
					mSelectedActor->move(
						glm::vec3(0.0, 0.0, 1.0) * evt.dy * smoothFactor
						);
					statsChanged  = true;
				}
			}
			else{
				wt::RaycastHitEvent res;

				if(mPhysics->pick(mScene->getCamera(), mSceneView->getRenderer().getFrustum(),
					glm::vec2(evt.x, evt.y), glm::vec2(mSceneView->width(), mSceneView->height()), res)){
						// TODO handle this better 
						if(mSelectedActor != res.mPickedActor->getSceneActor()->getUserData()){
							// condition checks if the hit actor isn't the selected actor (prevents the user from moving the actor to a point that's on itself)
							mSelectedActor->setPosition(
								res.mImpact);
							statsChanged  = true;
						}
				}
			}
		}
	}
	if(statsChanged){
		updateSelectionStats();
	}
}

void ActorEditTool::onSceneInitialized(){
	 for(wt::EntityManager::EntityMap::iterator i=mGameLevel->getEntityManager()->getActors().begin(); i!=mGameLevel->getEntityManager()->getActors().end(); i++){
		 addActor(i->second);
	 }
}

void ActorEditTool::onAnimationSelected(QString s){
	//if(mSelectedActor && mSelectedActor->hasAnimation()){
	//	mSelectedActor->getAnimationPlayer()->play(s.toStdString(), true);
	//}
}

void ActorEditTool::onToggleBrush(){
	if(isToolFocused()){
		getToolManager()->giveupFocus(this);
	}
	else{
		getToolManager()->requestFocus(this);
	}
}

void ActorEditTool::selectActor(wt::AGameActor* actor){
	mSelectedActor = actor;

	//wt::Scene& scene = *mScene;

	//glm::vec3 dir = glm::normalize(actor->getPosition() - mScene->getCamera().getPosition());
	//float distance = glm::length(actor->getPosition() - mScene->getCamera().getPosition());
	//const float d=50.0f;
	//if(distance > d){
	//	mScene->getCamera().setPosition(
	//		actor->getPosition() - dir*d);
	//}
	//mScene->getCamera().lookAt(actor->getPosition());

	//ui.comboBoxAnimation->clear();
	//for(wt::Model::AnimationMap::iterator i=actor->getModel()->getAnimations().begin(); i!=actor->getModel()->getAnimations().end(); i++){
	//	ui.comboBoxAnimation->addItem(i->first.c_str());
	//}

	updateSelectionStats();
}

void ActorEditTool::addActor(wt::AGameActor* actor){
	mGameLevel->addActor(actor);
}

void ActorEditTool::onNewActor(){
	ActorCreationDialog::EditResult res = ActorCreationDialog::edit(this, mGameLevel->getAssets());
	if(res.ok){
		wt::SceneActor* sceneActor;

		// Create scene actor
		sceneActor = mGameLevel->getScene()->createActor(res.name.toStdString());

		sceneActor->setModel(res.model, res.skin->getName());

		sceneActor->getTransform().setPosition(
			mGameLevel->getScene()->getCamera().getPosition() + mGameLevel->getScene()->getCamera().getForwardVec()*10.0f);

		// Create physics actor
		if(res.isControlled){
			res.physicsDesc.pose.setPosition( sceneActor->getTransform().getPosition() );
			mPhysics->createActor(sceneActor, res.physicsDesc);
		}

		wt::Doodad* doodad = new wt::Doodad(sceneActor, res.name.toStdString(), mGameLevel->getAssets());

		
		addActor(doodad);
	}
}