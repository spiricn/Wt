#include "stdafx.h"

#include <qspinbox.h>

#include "wte/ActorCreationDialog.h"
#include "wte/ResourcePickerDialog.h"

#include <wt/Model.h>

#define TD_TRACE_TAG "ActorCreationDialog"

ActorCreationDialog::ActorCreationDialog(QWidget* parent, wt::AResourceSystem* assets) : QDialog(parent), mAssets(assets){
    ui.setupUi(this);

	mResult.ok = false;

	onGeometryChanged(0);
}

void ActorCreationDialog::setModel(wt::Model* model){
}

void ActorCreationDialog::onActorTypeChanged(int type){
	ui.stackedWidget->setCurrentIndex(type);
}

void ActorCreationDialog::onSoundPick(){
	wt::ASoundBuffer* sound= ResourcePickerDialog::pickResource<wt::ASoundBuffer>(this, mAssets->getSoundManager());
	if(sound){
		ui.sound->setText(sound->getPath().c_str());
		mResult.sound3D.soundBuffer = sound;
	}
}

void ActorCreationDialog::onParticlePick(){
	wt::ParticleEffectResource* effect = ResourcePickerDialog::pickResource<wt::ParticleEffectResource>(this, mAssets->getParticleResourceManager());
	if(effect){
		ui.particleEffect->setText(effect->getPath().c_str());
		mResult.particleEffect.effect = effect;
	}
}

void ActorCreationDialog::onModelPick(){
	
}

void ActorCreationDialog::onGeometryChanged(int index){
}


void ActorCreationDialog::onSave(){
	int actorType = ui.actorType->currentIndex();

	mResult.name = ui.lineEditName->text();

	// Modelled actor
	if(actorType == 0){
		mResult.type = wt::ASceneActor::eTYPE_MODELLED;
		mResult.ok = true;
	}
	// Particle effect
	else if(actorType == 1){
		mResult.type = wt::ASceneActor::eTYPE_PARTICLE_EFFECT;
		mResult.ok = true;
	}
	else if(actorType == 2){
		mResult.type = wt::ASceneActor::eTYPE_POINT_LIGHT;
		mResult.ok = true;
	}
	else if(actorType == 3){
		mResult.type = wt::ASceneActor::eTYPE_SOUND;
		mResult.ok = true;
	}
	else if(actorType == 4){
		mResult.type = wt::ASceneActor::eTYPE_COLLIDER;
		mResult.ok = true;
	}
	else{
		WT_THROW("Unsupported actor type");
	}

	close();
}