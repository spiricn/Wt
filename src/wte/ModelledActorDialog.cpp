#include "stdafx.h"

#include "wte/ModelledActorDialog.h"

#include "wte/ResourcePickerDialog.h"

#include <wt/Scene.h>

#define TD_TRACE_TAG "ModelledActorDialog"

ModelledActorDialog::ModelledActorDialog(QWidget* parent, wt::ModelledActor* actor) : QDialog(parent), mActor(actor){
    ui.setupUi(this);
	
	ui.physicsGroup->setChecked( mActor->getPhysicsActor() != NULL );

	ui.boxExtents->setValue(glm::vec3(1.0f, 1.0f, 1.0f));

	if(mActor->getPhysicsActor()){
		const wt::PhysicsActor::Desc& desc = mActor->getPhysicsActor()->getDesc();

		ui.isDynamic->setChecked( desc.type == wt::PhysicsActor::eTYPE_DYNAMIC );


		if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_BOX){
			ui.geometry->setCurrentIndex(0);

			ui.boxExtents->setValue(glm::vec3(desc.geometryDesc.boxGeometry.hx,
				desc.geometryDesc.boxGeometry.hy,
				desc.geometryDesc.boxGeometry.hz));
		}
		else if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_SPHERE){
			ui.geometry->setCurrentIndex(1);

			ui.sphereRadius->setValue(desc.geometryDesc.sphereGeometry.radius);
		}
		else if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_MESH){
			ui.geometry->setCurrentIndex(2);
		}
		else{
			WT_THROW("Unsupported geometry type");
		}
	}

	if(mActor->getModel()){
		setModel(mActor->getModel());

		int skinIdx = 0;
		for(wt::Model::SkinMap::iterator i=mActor->getModel()->getSkins().begin(); i!=mActor->getModel()->getSkins().end(); i++){
			if(i->second == mActor->getSkin()){
				ui.comboBoxSkin->setCurrentIndex(skinIdx);
				break;
			}
			else{
				++skinIdx;
			}
		}
	}
}

void ModelledActorDialog::setModel(wt::Model* model){
	mResult.visual.model = model;

	ui.lineEditModel->setText( model->getPath().c_str() );

	ui.comboBoxSkin->clear();

	for(wt::Model::SkinMap::iterator i=model->getSkins().begin(); i!=model->getSkins().end(); i++){
		ui.comboBoxSkin->addItem(
			i->first.c_str(), QVariant(i->first.c_str()));
	}
}

void ModelledActorDialog::onSetModel(){
	wt::Model* model = ResourcePickerDialog::pickResource<wt::Model>(this, mActor->getScene()->getAssets()->getModelManager() );

	if(model){
		setModel(model);
	}
}

ModelledActorDialog::Result ModelledActorDialog::edit(QWidget* parent, wt::ModelledActor* actor){
	ModelledActorDialog dlg(parent, actor);
	dlg.exec();

	return dlg.mResult;
}

void ModelledActorDialog::onSave(){
	// Physics
	mResult.physics.enabled = ui.physicsGroup->isChecked();

	if(mResult.physics.enabled){
		wt::PhysicsActor::Desc& pd = mResult.physics.desc;
		pd.controlMode = wt::PhysicsActor::eCTRL_MODE_PHYSICS;
		pd.type = ui.isDynamic->isChecked() ? wt::PhysicsActor::eTYPE_DYNAMIC : wt::PhysicsActor::eTYPE_STATIC;
		int geometry = ui.geometry->currentIndex();

		// Box geometry
		if(geometry == 0){
			pd.geometryType = wt::PhysicsActor::eGEOMETRY_BOX;
			
			pd.geometryDesc.boxGeometry.hx = ui.boxExtents->getX();
			pd.geometryDesc.boxGeometry.hy = ui.boxExtents->getY();
			pd.geometryDesc.boxGeometry.hz = ui.boxExtents->getZ();
		}
		// Sphere geometry
		else if(geometry == 1){
			pd.geometryType = wt::PhysicsActor::eGEOMETRY_SPHERE;

			pd.geometryDesc.sphereGeometry.radius = ui.sphereRadius->value();
		}
		// Mesh geometry
		else if(geometry == 2){
			pd.geometryType = wt::PhysicsActor::eGEOMETRY_MESH;
			pd.geometryDesc.meshGeometry.model = mActor->getModel();
		}
		else{
			pd.geometryType = wt::PhysicsActor::eGEOMETRY_INVALID;
		}
	}

	// Visual (model already set via setModel function)
	wt::String skinName = ui.comboBoxSkin->itemData( ui.comboBoxSkin->currentIndex(), Qt::UserRole).toString().toStdString();
	mResult.visual.skin = mResult.visual.model->getSkin(skinName);

	mResult.accepted = true;

	close();
}

void ModelledActorDialog::onGeometryTypeChanged(int idx){
	ui.stackedWidget->setCurrentIndex(idx);
}
