#ifndef WTE_ACTORCREATIONDIALOG_H
#define WTE_ACTORCREATIONDIALOG_H

#include "ui_ActorCreationDialog.h"

#include <wt/ModelledActor.h>
#include <wt/Physics.h>
#include <wt/Assets.h>

namespace wt{
	class Model;
	class Model::GeometrySkin;
}

class ActorCreationDialog : public QDialog{
Q_OBJECT

public:
	struct EditResult{
		bool ok;
		wt::Model* model;
		wt::Model::GeometrySkin* skin;
		wt::PhysicsActor::ActorType type;
		wt::PhysicsActor::Desc physicsDesc;
		bool isControlled;
		QString name;
	};

	EditResult mResult;

private:
	wt::Assets* mAssets;
    Ui::ActorCreationDialog ui;

	void setModel(wt::Model* model);

	void updateTabs(){
		//int ctrl = ui.comboBoxCtrlMode->currentIndex();
	}

public:
    ActorCreationDialog(QWidget* parent, wt::Assets* assets);

	static EditResult edit(QWidget* parent,  wt::Assets* assets, wt::ModelledActor* actor=NULL){
		ActorCreationDialog dlg(parent, assets);
		dlg.exec();

		return dlg.mResult;
	}

protected slots:

	void onModelPick();

	void onSave();

}; // </ActorCreationDialog>

#endif // </WTE_ACTORCREATIONDIALOG_H>