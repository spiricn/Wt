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

void ActorCreationDialog::onGeometryChanged(int index){
	ui.boxGeometryGroup->setEnabled(index==2);
	ui.sphereGeometryGroup->setEnabled(index==1);
}


void ActorCreationDialog::onSave(){
	wt::String skinName = ui.comboBoxSkin->itemData( ui.comboBoxSkin->currentIndex(), Qt::UserRole).toString().toStdString();
	
	mResult.ok = true;
	mResult.skin = mResult.model->getSkin(skinName);
	mResult.type = ui.isDynamic->isChecked() ? wt::PhysicsActor::eDYNAMIC_ACTOR : wt::PhysicsActor::eSTATIC_ACTOR;
	mResult.name = ui.lineEditName->text();
	
	wt::PhysicsActor::Desc& pd = mResult.physicsDesc;

	pd.type = mResult.type;

	
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
		pd.geometryDesc.meshGeometry.model = mResult.model;
	}
	else{
		pd.geometryType = wt::PhysicsActor::eGEO_TYPE_NONE;
	}

	close();
}