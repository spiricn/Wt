#include "stdafx.h"

#include <qspinbox.h>

#include "wte/ActorCreationDialog.h"
#include "wte/ResourcePickerDialog.h"

#include <wt/Model.h>

const char* ActorCreationDialog::TAG = "ActorCreationDialog";

ActorCreationDialog::ActorCreationDialog(QWidget* parent, wt::Assets* assets) : QDialog(parent), mAssets(assets){
    ui.setupUi(this);

	mResult.ok = false;
}

void ActorCreationDialog::setModel(wt::Model* model){
	mResult.model = model;

	ui.lineEditModel->setText( model->getPath().c_str() );

	ui.comboBoxSkin->clear();
	for(wt::Model::SkinMap::iterator i=model->getSkins().begin(); i!=model->getSkins().end(); i++){
		ui.comboBoxSkin->addItem(
			i->first.c_str(), QVariant(i->first.c_str()));
	}
}

void ActorCreationDialog::onModelPick(){
	wt::Model* model = ResourcePickerDialog::pickResource<wt::Model>(this, mAssets->getModelManager()); // TODO
	if(model){
		setModel(model);
	}
}

void ActorCreationDialog::onSave(){
	//wt::String skinName = ui.comboBoxSkin->itemData( ui.comboBoxSkin->currentIndex(), Qt::UserRole).toString().toStdString();
	//
	//mResult.ok = true;
	//mResult.skin = mResult.model->getSkin(skinName);
	//mResult.type = ui.comboBoxType->currentIndex()==0 ? wt::PhysicsActor::eSTATIC_ACTOR : wt::PhysicsActor::eDYNAMIC_ACTOR;
	//mResult.name = ui.lineEditName->text();
	//mResult.isControlled = true;

	//{
	//wt::PhysicsActor::Desc& pd = mResult.physicsDesc;

	//pd.type = mResult.type;

	//int ctrlMode = ui.comboBoxCtrlMode->currentIndex();
	//if(ctrlMode == 0){ // not controlled
	//	mResult.isControlled = false;
	//}
	//else if(ctrlMode == 1){ // physics
	//	pd.controlMode = wt::PhysicsActor::ePHYSICS_MODE;

	//	int geometry = ui.comboBoxGeometry->currentIndex();

	//	if(geometry == 1){ // box
	//		pd.geometryType = wt::PhysicsActor::eBOX_GEOMETRY;

	//		std::stringstream ss(
	//			ui.boxGeometryExtents->text().toStdString()
	//			);
	//		
	//		ss >> pd.geometryDesc.boxGeometry.hx
	//			>>  pd.geometryDesc.boxGeometry.hy
	//			 >> pd.geometryDesc.boxGeometry.hz;

	//	}
	//	else if(geometry == 2){ // sphere
	//		pd.geometryType = wt::PhysicsActor::eSPHERE_GEOMETRY;

	//		pd.geometryDesc.sphereGeometry.radius = ui.sphereGeometryRadius->value();

	//	}
	//	else if(geometry == 3){ // mesh
	//		pd.geometryType = wt::PhysicsActor::eMESH_GEOMETRY;
	//		pd.geometryDesc.meshGeometry.model = mResult.model;
	//	}
	//	else{ // none
	//	}
	//}
	//else{ // controller
	//	int ctrlType = ui.comboBoxCtrlType->currentIndex();

	//	if(ctrlType == 0){ // box
	//		pd.controllerDesc.geometryType = wt::PhysicsActor::eBOX_CONTROLLER;
	//	}
	//	else{ // capsule
	//		pd.controllerDesc.geometryType = wt::PhysicsActor::eCAPSULE_CONTROLLER;
	//	}
	//}

	//}
	//close();
}