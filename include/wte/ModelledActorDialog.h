#ifndef WTE_PHYSICSACTORDIALOG_H
#define WTE_PHYSICSACTORDIALOG_H

#include "ui_ModelledActorDialog.h"

#include <wt/PhysicsActor.h>
#include <wt/ModelledActor.h>

class ModelledActorDialog : public QDialog{
Q_OBJECT

public:
	struct Result{
		bool accepted;

		struct{
			bool enabled;
			wt::PhysicsActor::Desc desc;
		} physics;

		struct Visual{
			wt::Model* model;
			wt::ModelSkin* skin;

			Visual() : model(NULL), skin(NULL){
			}
		} visual;

		Result() : accepted(false){
		}

	}; // </Result>

	static Result edit(QWidget* parent, wt::ModelledActor* actor);

protected slots:

	void onSave();

	void onSetModel();

private:
	ModelledActorDialog(QWidget* parent, wt::ModelledActor* actor);

	void setModel(wt::Model* model);

	Result mResult;
	wt::ModelledActor* mActor;
    Ui::ModelledActorDialog ui;


}; // </PhysicsActorDialog>

#endif // </WTE_PHYSICSACTORDIALOG_H>