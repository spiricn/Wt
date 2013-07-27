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
		wt::ASceneActor::ActorType type;
		QString name;

		bool ok;

		struct {
			wt::Model* model;
			wt::Model::GeometrySkin* skin;
			wt::PhysicsActor::ActorType type;
			wt::PhysicsActor::Desc physicsDesc;
		} modelledActor;

		struct{
			wt::ParticleEffectResource* effect;
		} particleEffect;
	};

	EditResult mResult;

public:
    ActorCreationDialog(QWidget* parent, wt::AResourceSystem* assets);

	static EditResult edit(QWidget* parent,  wt::AResourceSystem* assets, wt::ASceneActor* actor=NULL){
		ActorCreationDialog dlg(parent, assets);
		dlg.exec();

		return dlg.mResult;
	}

protected slots:
	void onSave();

	void onModelPick();

	void onGeometryChanged(int);

	void onParticlePick();

	void onActorTypeChanged(int);

private:
	wt::AResourceSystem* mAssets;
    Ui::ActorCreationDialog ui;

	void setModel(wt::Model* model);

}; // </ActorCreationDialog>

#endif // </WTE_ACTORCREATIONDIALOG_H>