#ifndef WTE_DLGSKINNEDMODELEDIT_H
#define WTE_DLGSKINNEDMODELEDIT_H

#include <qdialog.h>

#include <wt/SkyBox.h>
#include <wt/ImageManager.h>
#include <Wt/ModelManager.h>
#include <Wt/Model.h>
#include <Wt/Animation.h>
#include <wt/Assets.h>

#include "ui_ModelEditDialog.h"

#include "wte/ResourcePickerDialog.h"
#include "wte/SkinEditDialog.h"

class ModelEditDialog : public QDialog{
Q_OBJECT
public:
	struct EditResult{
		QString name;

		
		typedef QPair<QString, wt::Animation*> Animation;
		QVector<Animation> animations;

		bool ok;

		EditResult() : name(""), ok(false){
		}
	};

private:
	Ui::ModelEditDialog ui;
	EditResult mResult;
	wt::Model* mModel;
	wt::Assets* mAssets;

public:

	ModelEditDialog(QWidget* parent, wt::Assets* assets, wt::Model* model) : QDialog(parent), mModel(model), mAssets(assets){
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

	void addAnimation(const QString& name, wt::Animation* ani){
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.animationTree);
		item->setText(0, name);
		item->setText(1, ani->getPath().c_str());

		ui.animationTree->addTopLevelItem(item);
	}

	static EditResult edit(QWidget* parent, wt::Assets* assets, wt::Model* model=NULL){
		ModelEditDialog dlg(parent, assets, model);

		dlg.exec();

		return dlg.mResult;
	}

protected slots:
	void onSave(){
		mResult.name = ui.nameEdit->displayText();
		mResult.ok = true;
		close();
	}

	void onSkinActivated(void* item){
		SkinEditDialog::edit(this, mAssets, (wt::Model::GeometrySkin*)item);
	}

	void onAddAnimation(){
		bool ok;
		QString name = QInputDialog::getText(this, "Input",
			"Animation name: ", QLineEdit::Normal, "", &ok);
		if(!ok){
			return;
		}

		ResourcePickerDialog picker(this);
		wt::Animation* ani = picker.pickResource<wt::Animation>(this, mAssets->getAnimationManager());

		if(ani){
			if(mModel->hasAnimation(name.toStdString()) || mModel->hasAnimation(ani)){
				LOGE("ModelEditDialog", "Model already contains this animation");
			}
			else{
				mModel->addSkeletalAnimation(name.toStdString(), ani);
				addAnimation(name, ani);
			}
		}
	}
	
	void onDeleteSkin(){
		wt::Model::GeometrySkin* skin = (wt::Model::GeometrySkin*)ui.skinTree->getSelectedItem();

		mModel->deleteSkin(skin);
		ui.skinTree->deleteItem(skin);
	}

	void onAddSkin(){
		bool ok;
		QString name = QInputDialog::getText(this, "Input",
			"Skin name: ", QLineEdit::Normal, "", &ok);
		if(!ok){
			return;
		}

		ui.skinTree->addItem(name, mModel->createSkin(name.toStdString()));
	}

}; // </ModelEditDialog>

#endif // </WTE_DLGSKINNEDMODELEDIT_H>