#include "stdafx.h"

#include "wte/DoodadEditDialog.h"
#include "wte/ResourcePickerDialog.h"

const char* DoodadEditDialog::TAG = "DoodadEditDialog";

DoodadEditDialog::DoodadEditDialog(QWidget* parent) : QDialog(parent){
    ui.setupUi(this);
}

void DoodadEditDialog::setModel(wt::Model* model){
	mResult.model = model;

	ui.modelPath->setText( model->getPath().c_str() );

	ui.modelSkin->clear();
	for(wt::Model::SkinMap::iterator i=model->getSkins().begin(); i!=model->getSkins().end(); i++){
		ui.modelSkin->addItem(
			i->first.c_str(), QVariant(i->first.c_str()));
	}
}

void DoodadEditDialog::onPickModel(){
	wt::Model* model = NULL; // ResourcePickerDialog::pickResource<wt::Model>(this, &wt::ModelManager::getSingleton()); // TODO
	if(model){
		setModel(model);
	}
}

void DoodadEditDialog::onOk(){
	mResult.ok = true;
	mResult.skin = ui.modelSkin->currentText();

	close();
}