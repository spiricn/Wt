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
	mResult.modelledActor.model = model;

	ui.lineEditModel->setText( model->getPath().c_str() );

	ui.comboBoxSkin->clear();
	for(wt::Model::SkinMap::iterator i=model->getSkins().begin(); i!=model->getSkins().end(); i++){
		ui.comboBoxSkin->addItem(
			i->first.c_str(), QVariant(i->first.c_str()));
	}
}

void ActorCreationDialog::onActorTypeChanged(int type){
	// :(
	ui.stackedWidget->setCurrentIndex(
		// Model
		type == 0 ? 2 :
		// Particle
		type == 1 ? 0 :
		// Light
		1
	);
}

void ActorCreationDialog::onParticlePick(){
	wt::ParticleEffectResource* effect = ResourcePickerDialog::pickResource<wt::ParticleEffectResource>(this, mAssets->getParticleResourceManager());
	if(effect){
		ui.particleEffect->setText(effect->getPath().c_str());
		mResult.particleEffect.effect = effect;
	}
}

void ActorCreationDialog::onModelPick(){
	wt::Model* model = ResourcePickerDialog::pickResource<wt::Model>(this, mAssets->getModelManager()); // TODO
	if(model){
		setModel(model);
	}
}

void ActorCreationDialog::onGeometryChanged(int index){
	ui.boxGeometryGroup->setEnabled(index==2);
	ui.sphereGeometryGroup->setEnabled(index==1);
}


void ActorCreationDialog::onSave(){
	int actorType = ui.actorType->currentIndex();

	mResult.name = ui.lineEditName->text();

	// Modelled actor
	if(actorType == 0){
		mResult.type = wt::ASceneActor::eTYPE_MODELLED;
		wt::String skinName = ui.comboBoxSkin->itemData( ui.comboBoxSkin->currentIndex(), Qt::UserRole).toString().toStdString();
	
		mResult.ok = true;
		mResult.modelledActor.skin = mResult.modelledActor.model->getSkin(skinName);
		mResult.modelledActor.type = ui.isDynamic->isChecked() ? wt::PhysicsActor::eDYNAMIC_ACTOR : wt::PhysicsActor::eSTATIC_ACTOR;
	
		wt::PhysicsActor::Desc& pd = mResult.modelledActor.physicsDesc;

		pd.type = mResult.modelledActor.type;

	
		pd.controlMode = wt::PhysicsActor::ePHYSICS_MODE;

		int geometry = ui.geometry->currentIndex();

		// Box geometry
		if(geometry == 1){
			pd.geometryType = wt::PhysicsActor::eBOX_GEOMETRY;
			
			pd.geometryDesc.boxGeometry.hx = ui.boxHx->value();
			pd.geometryDesc.boxGeometry.hy = ui.boxHy->value();
			pd.geometryDesc.boxGeometry.hz = ui.boxHz->value();
		}
		// Sphere geometry
		else if(geometry == 2){
			pd.geometryType = wt::PhysicsActor::eSPHERE_GEOMETRY;

			pd.geometryDesc.sphereGeometry.radius = ui.sphereRadius->value();
		}
		// Mesh geometry
		else if(geometry == 3){
			pd.geometryType = wt::PhysicsActor::eMESH_GEOMETRY;
			pd.geometryDesc.meshGeometry.model = mResult.modelledActor.model;
		}
		else{
			pd.geometryType = wt::PhysicsActor::eGEO_TYPE_NONE;
		}
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

	close();
}