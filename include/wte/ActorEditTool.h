#ifndef WTE_ACTOREDITTOOL_H
#define WTE_ACTOREDITTOOL_H

#include "ui_ActorEditTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

#include <wte/ParticleEditDialog.h>

class ActorEditTool : public QDialog, public ATool{
Q_OBJECT

public:
	virtual void onToolLostFocus(){
		ATool::onToolLostFocus();

		ui.buttonBrushToggle->setText("Activate brush");
	}

	virtual void onToolGainFocus(){
		ATool::onToolGainFocus();

		ui.buttonBrushToggle->setText("Deactivate brush");
	}

    ActorEditTool(SceneView* sceneView, QWidget* parent, AToolManager*, wt::AResourceSystem* assets);

private:
    Ui::ActorEditTool ui;
	SceneView* mSceneView;
	wt::Scene* mScene;
	wt::Physics* mPhysics;
	wt::AResourceSystem* mAssets;

	ParticleEditDialog* mParticleEditDialog;

	wt::ASceneActor* mSelectedActor;

	void selectActor(wt::ASceneActor* actor);
	
	void updateSelectionStats();

	bool mSelectingActor;

protected slots:

	void onNewActor();

	void onAnimationSelected(QString);

	void onMouseDrag(MouseDragEvent evt);

	void onToggleBrush();

	void onSelectActor();

	void onScaleChanged();

	void onTranslationChanged();

	void onEditParticleEffect();

	void onRotationChanged();

	void onMousePress(QMouseEvent*);

	void onDeleteActor();

	void onBBoxGeometryChanged();

}; // </ActorEditTool>

#endif