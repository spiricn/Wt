#ifndef WTE_ACTOREDITTOOL_H
#define WTE_ACTOREDITTOOL_H

#include "ui_ActorEditTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

#include <wt/game/AGameActor.h>
#include <wt/game/GameLevel.h>

class ActorEditTool : public QDialog, public ATool{
Q_OBJECT

private:
    Ui::ActorEditTool ui;
	SceneView* mSceneView;
	wt::Scene* mScene;
	wt::Physics* mPhysics;

	void addActor(wt::AGameActor* actor);

	void selectActor(wt::AGameActor* actor);

	wt::AGameActor* mSelectedActor;

	void updateSelectionStats();

	wt::GameLevel* mGameLevel;

	bool mSelectingActor;

public:

	virtual void onToolLostFocus(){
		ATool::onToolLostFocus();

		ui.buttonBrushToggle->setText("Activate brush");
	}

	virtual void onToolGainFocus(){
		ATool::onToolGainFocus();

		ui.buttonBrushToggle->setText("Deactivate brush");
	}

    ActorEditTool(SceneView* sceneView, wt::GameLevel* level, QWidget* parent, AToolManager*);

protected slots:

	void onSceneInitialized();

	void onNewActor();

	void onAnimationSelected(QString);

	void onMouseDrag(MouseDragEvent evt);

	void onToggleBrush();

	void onSave();

	void onAddDoodad();

	void onSelectActor();

	void onScaleChanged();

	void onTranslationChanged();

	void onRotationChanged();

	void onMousePress(QMouseEvent*);

}; // </ActorEditTool>

#endif