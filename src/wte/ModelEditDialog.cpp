#include "stdafx.h"
#include "wte/ModelEditDialog.h"

#define TD_TRACE_TAG "ModelEditDialog"

ModelEditDialog::ModelEditDialog(QWidget* parent, wt::AResourceSystem* assets, wt::Model* model) : QDialog(parent), mModel(model), mAssets(assets){
	ui.setupUi(this);


	ui.nameEdit->setText(model->getName().c_str());

	for(wt::Model::AnimationMap::iterator i=model->getAnimations().begin();
		i!=model->getAnimations().end(); i++){
			addAnimation(i->first.c_str(), i->second->getAnimation());
	}

	for(wt::Model::SkinMap::iterator i=model->getSkins().begin(); i!=model->getSkins().end(); i++){
		ui.skinTree->addItem(
			i->first.c_str(), i->second);
	}

	connect(ui.skinTree, SIGNAL(onItemActivated(void*)),
		this, SLOT(onSkinActivated(void*))
		);
}

void ModelEditDialog::addAnimation(const QString& name, wt::Animation* ani){
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.animationTree);
	item->setText(0, name);
	item->setText(1, ani->getPath().c_str());

	ui.animationTree->addTopLevelItem(item);
}

ModelEditDialog::EditResult ModelEditDialog::edit(QWidget* parent, wt::AResourceSystem* assets, wt::Model* model){
	ModelEditDialog dlg(parent, assets, model);

	dlg.exec();

	return dlg.mResult;
}

void ModelEditDialog::onSave(){
	mResult.name = ui.nameEdit->displayText();
	mResult.ok = true;
	close();
}

void ModelEditDialog::onSkinActivated(void* item){
	SkinEditDialog::edit(this, mAssets, (wt::ModelSkin*)item);
}

void ModelEditDialog::onAddAnimation(){
	bool ok;
	QString name = QInputDialog::getText(this, "Input",
		"Animation name: ", QLineEdit::Normal, "", &ok);
	if(!ok){
		return;
	}

	wt::Animation* ani = ResourcePickerDialog::pickResource<wt::Animation>(this, mAssets->getAnimationManager());

	if(ani){
		if(mModel->hasAnimation(name.toStdString()) || mModel->hasAnimation(ani)){
			TRACEE("Model already contains animation named \"%s\"", name.toStdString().c_str());
		}
		else{
			mModel->addSkeletalAnimation(name.toStdString(), ani);
			addAnimation(name, ani);
		}
	}
}

void ModelEditDialog::onAddFromGroup(){
	QList<wt::Animation*> animations;
	ResourcePickerDialog::pickMultiResources<wt::Animation>(this, WTE_CTX.getAssets()->getAnimationManager(), animations, "Pick animations");

	for(QList<wt::Animation*>::Iterator iter=animations.begin(); iter!=animations.end(); iter++){
		wt::Animation* animation = *iter;

		if(mModel->hasAnimation(animation->getName())){
			TRACEE("Model already contains animation named \"%s\"", animation->getName().c_str());
			continue;
		}

		mModel->addSkeletalAnimation(animation->getName(), animation);
		addAnimation(animation->getName().c_str(), animation);
	}
}
	
void ModelEditDialog::onDeleteSkin(){
	wt::ModelSkin* skin = (wt::ModelSkin*)ui.skinTree->getSelectedItem();

	mModel->deleteSkin(skin);
	ui.skinTree->deleteItem(skin);
}

void ModelEditDialog::onAddSkin(){
	bool ok;
	QString name = QInputDialog::getText(this, "Input",
		"Skin name: ", QLineEdit::Normal, "", &ok);
	if(!ok){
		return;
	}

	ui.skinTree->addItem(name, mModel->createSkin(name.toStdString()));
}