#include "stdafx.h"

#include "wte/ModelledActorDialog.h"

#include "wte/ResourcePickerDialog.h"

#include <wt/Scene.h>

#define TD_TRACE_TAG "ModelledActorDialog"

ModelledActorDialog::ModelledActorDialog(QWidget* parent, wt::ModelledActor* actor) : QDialog(parent), mActor(actor){
    ui.setupUi(this);
	
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

	// Visual (model already set via setModel function)
	wt::String skinName = ui.comboBoxSkin->itemData( ui.comboBoxSkin->currentIndex(), Qt::UserRole).toString().toStdString();
	mResult.visual.skin = mResult.visual.model->getSkin(skinName);

	mResult.accepted = true;

	close();
}
