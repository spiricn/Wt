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

	connect(mSceneView, SIGNAL(onMouseDrag(MouseDragEvent)),
		this, SLOT(onMouseDrag(MouseDragEvent)));

	connect(mSceneView, SIGNAL(onMouseDown(QMouseEvent*)),
		 this, SLOT(onMousePress(QMouseEvent*)));
	
	mScene = sceneView->getScene();
	mPhysics = mScene->getPhysics();

	mParticleEditDialog = new ParticleEditDialog(this, NULL);

	ui.lightAmbientIntensity->setValueRange(0, 100);
	ui.lightDiffuseIntensity->setValueRange(0, 100);

	ui.lightDiffuseIntensity->setSingleStep(0.01);
	ui.lightAmbientIntensity->setSingleStep(0.01);

	ui.lightAttenuation->setDecimals(4);

	// C
	ui.lightAttenuation->setSingleStep(0.001, VecEditWidget::eFIELD_X);
	ui.lightAttenuation->setValueRange(0, 1.0f, VecEditWidget::eFIELD_X);

	// L
	ui.lightAttenuation->setSingleStep(0.01, VecEditWidget::eFIELD_Y);
	ui.lightAttenuation->setValueRange(0, 50.0f, VecEditWidget::eFIELD_Y);

	// Q
	ui.lightAttenuation->setSingleStep(0.001, VecEditWidget::eFIELD_Z);
	ui.lightAttenuation->setValueRange(0, 1.0f, VecEditWidget::eFIELD_Z);

	ui.volume->setValueRange(0, 100);
	ui.minDistance->setValueRange(0.5, 100);
	ui.attenuation->setValueRange(0, 100);
	ui.pitch->setValueRange(0.01, 10);

	selectActor(NULL);
}

void ActorEditTool::onScaleChanged(){
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransformable()->setScale(
			ui.transform->getScale()
		);
	}
}

void ActorEditTool::onBeforeSceneUnload(){
	selectActor(NULL);
}


void ActorEditTool::onTranslationChanged(){
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransformable()->setTranslation(
			ui.transform->getTranslation()
		);
	}
}

void ActorEditTool::onRotationChanged(){
	if(isToolFocused() && mSelectedActor){
		mSelectedActor->getTransformable()->setRotation(
			ui.transform->getRotation()
		);
	}
}

void ActorEditTool::onActorRename(){
	if(isToolFocused() && mSelectedActor){
		bool ok;
		wt::String name = QInputDialog::getText(this, "Input",
			"New name: ", QLineEdit::Normal, mSelectedActor->getName().c_str(), &ok).toStdString();

		if(!ok){
			return;
		}

		mSelectedActor->setName(name);
	}
}

void ActorEditTool::onMousePress(QMouseEvent* evt){
	if(!isToolFocused()){
		return;
	}

	if((evt->button() == Qt::LeftButton && mSelectingActor) || evt->button() == Qt::MiddleButton ){
		wt::RaycastHitEvent res;
		
		if(mPhysics->pick(mScene->getCamera(), glm::vec2(evt->x(), evt->y()), glm::vec2(mSceneView->width(), mSceneView->height()), res, 0, wt::Physics::ePICK_BOUNDING_BOXES)){
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

void ActorEditTool::onEditParticleEffect(){
	if(!isToolFocused() || !mSelectedActor || mSelectedActor->getActorType() != wt::ASceneActor::eTYPE_PARTICLE_EFFECT){
		return;
	}

	wt::ParticleEffect* e = static_cast<wt::ParticleEffect*>(mSelectedActor);
	mParticleEditDialog->setEffect(e->getEffectResource());

	mParticleEditDialog->show();
}

void ActorEditTool::updateSelectionStats(){
	if(!mSelectedActor){
		return;
	}

	// Transform
	glm::vec3 rotAxis;
	float rotAngle;
	glm::vec3 pos;
	glm::vec3 scale;

	mSelectedActor->getTransformable()->getRotation(rotAxis, rotAngle);
	mSelectedActor->getTransformable()->getTranslation(pos);
	mSelectedActor->getTransformable()->getScale(scale);

	ui.transform->setRotation(rotAxis, rotAngle);

	ui.transform->setTranslation( pos );

	ui.transform->setScale( scale );	
}

void ActorEditTool::onMouseDrag(MouseDragEvent evt){
	if(!isToolFocused()){
		return;
	}


	bool statsChanged = false;

	if(evt.button == Qt::LeftButton){
		if(mSelectedActor){

			float smoothFactor = 0.3;


			// TODO these are platform dependent
			bool shiftDown = wt::AGameInput::isKeyDown(VK_LSHIFT);

			bool altDown = wt::AGameInput::isKeyDown(VK_LMENU);

			if(wt::AGameInput::isKeyDown('X')){
				if(shiftDown){
					mSelectedActor->getTransformable()->rotate(
						glm::vec3(1.0, 0.0, 0.0), evt.dx * smoothFactor
					);

					statsChanged  = true;
				}
				else{
					mSelectedActor->getTransformable()->translate(
						glm::vec3(1.0, 0.0, 0.0) * evt.dx * smoothFactor
					);

					statsChanged  = true;
				}
			}
			else if(wt::AGameInput::isKeyDown('F')){
				mSelectedActor->getTransformable()->scale( glm::vec3(1, 1, 1) * evt.dy * 0.01f );
			}
			else if(wt::AGameInput::isKeyDown('Y')){
				if(shiftDown){
					mSelectedActor->getTransformable()->rotate(
						glm::vec3(0.0, 1.0, 0.0), evt.dx * smoothFactor
					);

					statsChanged  = true;
				}
				else{
					mSelectedActor->getTransformable()->translate(
						glm::vec3(0.0, 1.0, 0.0) * evt.dy * smoothFactor
					);

					statsChanged  = true;
				}
			}
			else if(wt::AGameInput::isKeyDown('Z')){
				if(shiftDown){
					mSelectedActor->getTransformable()->rotate(
						glm::vec3(0.0, 0.0, 1.0), evt.dx * smoothFactor
					);

					statsChanged  = true;
				}
				else{
					mSelectedActor->getTransformable()->translate(
						glm::vec3(0.0, 0.0, 1.0) * evt.dy * smoothFactor
					);

					statsChanged  = true;
				}
			}
			else if(altDown){
				wt::RaycastHitEvent res;

				if(mPhysics->pick(mScene->getCamera(),
					glm::vec2(evt.x, evt.y), glm::vec2(mSceneView->width(), mSceneView->height()), res, 0, wt::Physics::ePICK_TERRAIN)){
						// TODO handle this better 
						if(mSelectedActor != res.mPickedActor->getSceneActor()->getUserData()){
							// condition checks if the hit actor isn't the selected actor (prevents the user from moving the actor to a point that's on itself)
							mSelectedActor->getTransformable()->setTranslation(
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

void ActorEditTool::onBBoxGeometryChanged(){
}

void ActorEditTool::onDeleteActor(){
	if(mSelectedActor){
		mScene->deleteActor(mSelectedActor);
		selectActor(NULL);
	}
}

void ActorEditTool::onAnimationSelected(QString s){
	if(mSelectedActor && mSelectedActor->getActorType() == wt::ASceneActor::eTYPE_MODELLED){
		wt::ModelledActor* actor = static_cast<wt::ModelledActor*>(mSelectedActor);
		if(actor->hasAnimation()){
			if(ui.comboBoxAnimation->currentIndex()){
				actor->getAnimationPlayer()->play(s.toStdString(), true);
			}
			else{
				actor->getAnimationPlayer()->play(NULL);
			}
		}
	}
}

void ActorEditTool::onToggleBrush(){
	if(isToolFocused()){
		getToolManager()->giveupFocus(this);
	}
	else{
		getToolManager()->requestFocus(this);
	}
}

void ActorEditTool::onAmbientIntensityChanged(){
	const wt::PointLight* light = static_cast<wt::PointLight*>(mSelectedActor);

	wt::PointLight::Desc desc = light->getDesc();

	desc.ambientIntensity = ui.lightAmbientIntensity->getValue();

	light->setDesc(desc);
}

void ActorEditTool::onDiffuseIntensityChanged(){
	const wt::PointLight* light = static_cast<wt::PointLight*>(mSelectedActor);

	wt::PointLight::Desc desc = light->getDesc();

	desc.diffuseIntensity = ui.lightDiffuseIntensity->getValue();

	light->setDesc(desc);
}

void ActorEditTool::onLightColorChanged(){
	const wt::PointLight* light = static_cast<wt::PointLight*>(mSelectedActor);

	wt::PointLight::Desc desc = light->getDesc();

	desc.color = ui.lightColor->getColor();

	light->setDesc(desc);
}

void ActorEditTool::onAttenuationChanged(){
	wt::PointLight* light = static_cast<wt::PointLight*>(mSelectedActor);

	glm::vec3 val;
	ui.lightAttenuation->getValue(val);

	wt::PointLight::Desc desc = ((const wt::PointLight*)light)->getDesc();

	desc.attenuation.constant = val.x;
	desc.attenuation.linear = val.y;
	desc.attenuation.quadratic = val.z;

	light->setDesc(desc);
}

void ActorEditTool::selectActor(wt::ASceneActor* actor){
	if(mSelectedActor){
		mSelectedActor->setBoundingBoxColor(wt::Color::Green());
	}

	mSelectedActor = actor;

	ui.transform->setEnabled(actor!=NULL);

	if(!actor){
		return;
	}

	mSelectedActor->setBoundingBoxColor(wt::Color::Red());

	wt::Scene& scene = *mScene;

	glm::vec3 pos;
	mSelectedActor->getTransformable()->getTranslation(pos);

	glm::vec3 eyePos;
	mScene->getCamera().getTranslation(eyePos);

	/*glm::vec3 dir = glm::normalize(pos - eyePos);
	float distance = glm::length(pos - eyePos);
	const float d=50.0f;
	if(distance > d){
		mScene->getCamera().setTranslation(
			pos - dir*d);
	}

	mScene->getCamera().lookAt(pos);*/

	if(actor->getActorType() == wt::ASceneActor::eTYPE_MODELLED){
		// We don't want this to emit a signal
		ui.comboBoxAnimation->blockSignals(true);
		ui.comboBoxAnimation->clear();

		ui.comboBoxAnimation->setEnabled(true);
		ui.comboBoxAnimation->addItem("none");

		wt::ModelledActor* modelledActor = static_cast<wt::ModelledActor*>(actor);

		// Populate the animation combo box with the current actors animations
		uint32_t index = 1;
		int32_t activeIndex = -1;
		for(wt::Model::AnimationMap::iterator i=static_cast<wt::ModelledActor*>(actor)->getModel()->getAnimations().begin(); i!=static_cast<wt::ModelledActor*>(actor)->getModel()->getAnimations().end(); i++){
			ui.comboBoxAnimation->addItem(i->first.c_str());

			if(modelledActor->getAnimationPlayer()->getCurrentAnimation() == i->second){
				activeIndex = index;
			}

			++index;
		}

		ui.comboBoxAnimation->blockSignals(false);

		if(activeIndex > 0){
			ui.comboBoxAnimation->setCurrentIndex(activeIndex);
		}

		ui.stackedWidget->setCurrentIndex(0);

		ui.comboBoxAnimation->blockSignals(false);
	}
	else if(actor->getActorType() == wt::ASceneActor::eTYPE_PARTICLE_EFFECT){
		ui.stackedWidget->setCurrentIndex(1);
	}
	else if(actor->getActorType() == wt::ASceneActor::eTYPE_POINT_LIGHT){
		ui.stackedWidget->setCurrentIndex(2);

		const wt::PointLight* light = static_cast<const wt::PointLight*>(actor);
		
		ui.lightAttenuation->setValue(glm::vec3(
			light->getDesc().attenuation.constant,
			light->getDesc().attenuation.linear,
			light->getDesc().attenuation.quadratic
		));

		ui.lightColor->setColor(
			light->getDesc().color
		);

		ui.lightAmbientIntensity->setValue(
			light->getDesc().ambientIntensity
		);

		ui.lightDiffuseIntensity->setValue(
			light->getDesc().diffuseIntensity
		);
	}
	else if(actor->getActorType() == wt::ASceneActor::eTYPE_SOUND){
		ui.stackedWidget->setCurrentIndex(3);

		const wt::Sound* sound = dynamic_cast<const wt::Sound*>(actor);

		ui.volume->setValue( sound->getSound()->getVolume()*100.0f );

		ui.attenuation->setValue( sound->getSound()->getAttenuation() );

		ui.minDistance->setValue( sound->getSound()->getMinimumDistance() );

		ui.pitch->setValue( sound->getSound()->getPitch() );
	}
	else{
		WT_THROW("Unsupported actor type");
	}

	// Update tarnsform information
	updateSelectionStats();
}


void ActorEditTool::onNewActor(){
	ActorCreationDialog::EditResult res = ActorCreationDialog::edit(this, mAssets);

	wt::ASceneActor* sceneActor = NULL;

	glm::vec3 eyePos, eyeFw;

	mScene->getCamera().getTranslation(eyePos);

	mScene->getCamera().getForwardVector(eyeFw);

	if(res.ok){
		if(res.type == wt::ASceneActor::eTYPE_MODELLED){
			wt::ModelledActor* actor = mScene->createModelledActor(res.name.toStdString());

			actor->setModel(res.modelledActor.model, res.modelledActor.skin->getName());

			// Create physics actor
			if(res.modelledActor.physicsDesc.geometryType != wt::PhysicsActor::eGEO_TYPE_NONE){
				// Initial transform
				//actor->getTransformable()->getTransformMatrix(res.modelledActor.physicsDesc.pose);
				res.modelledActor.physicsDesc.pose = glm::translate(eyePos - eyeFw*3.0f);

				try{
					mPhysics->createActor(actor, res.modelledActor.physicsDesc);
				}catch(wt::Exception& e){
					QMessageBox::critical(this, "Error", 
						QString("Error creating physics actor\n\n") + e.getDescription().c_str());
				}
			}

			sceneActor = actor;
		}
		else if(res.type == wt::ASceneActor::eTYPE_PARTICLE_EFFECT){
			wt::ParticleEffect* actor;

			actor = mScene->createParticleEffect(res.name.toStdString());

			actor->create(res.particleEffect.effect);

			sceneActor = actor;
		}
		else if(res.type == wt::ASceneActor::eTYPE_POINT_LIGHT){
			const wt::PointLight* actor;

			wt::PointLight::Desc desc;
			actor = mScene->createPointLight(desc);


			sceneActor = (wt::ASceneActor*)actor;
		}
		else if(res.type == wt::ASceneActor::eTYPE_SOUND){
			wt::Sound* sound = mScene->createSound(res.name.toStdString()) ;

			sound->setSound( mAssets->getSoundSystem()->createSound(res.sound3D.soundBuffer) );
			sound->getSound()->setRelativeToListener( false );
			sound->getSound()->setLoop(true);
			sound->getSound()->play();

			sceneActor = dynamic_cast<wt::ASceneActor*>( sound );
		}
		else{
			WT_THROW("Unsupported actor type");
		}

		if(sceneActor){
			sceneActor->getTransformable()->setTranslation(
				 eyePos - eyeFw*10.0f);

			mPhysics->createBBox(sceneActor);
		}
	}
}

void ActorEditTool::onMinDistanceChanged(){
	if(isToolFocused() && mSelectedActor != NULL){
		wt::Sound* sound = dynamic_cast<wt::Sound*>(mSelectedActor);

		sound->getSound()->setMinimumDistance( ui.minDistance->getValue() );
	}
}

void ActorEditTool::onVolumeChanged(){
	if(isToolFocused() && mSelectedActor != NULL){
		wt::Sound* sound = dynamic_cast<wt::Sound*>(mSelectedActor);

		sound->getSound()->setVolume( ui.volume->getValue()/100.0f );
	}
}

void ActorEditTool::onSoundAttenuationChanged(){
	if(isToolFocused() && mSelectedActor != NULL){
		wt::Sound* sound = dynamic_cast<wt::Sound*>(mSelectedActor);

		sound->getSound()->setAttenuation( ui.attenuation->getValue() );
	}
}

void ActorEditTool::onPitchChanged(){
	if(isToolFocused() && mSelectedActor != NULL){
		wt::Sound* sound = dynamic_cast<wt::Sound*>(mSelectedActor);

		sound->getSound()->setPitch( ui.pitch->getValue() );
	}
}
