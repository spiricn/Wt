#ifndef WTE_ACTOREDITTOOL_H
#define WTE_ACTOREDITTOOL_H

#include "ui_ActorEditTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

class ActorEditTool : public QDialog, public ATool{
Q_OBJECT

private:
    Ui::ActorEditTool ui;
	SceneView* mSceneView;
	wt::Scene* mScene;
	wt::Physics* mPhysics;
	wt::AResourceSystem* mAssets;

	/*void addActor(wt::AGameActor* actor);

	*/

	void selectActor(wt::ASceneActor* actor);

	
	wt::ASceneActor* mSelectedActor;

	void updateSelectionStats();

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

    ActorEditTool(SceneView* sceneView, QWidget* parent, AToolManager*, wt::AResourceSystem* assets);

protected slots:

	void onNewActor();

	//void onSceneInitialized();

	//void onAnimationSelected(QString);

	//void onMouseDrag(MouseDragEvent evt);

	void onToggleBrush();

	//void onSave();

	//void onAddDoodad();

	void onSelectActor();

	void onScaleChanged();

	void onTranslationChanged();

	void onRotationChanged();

	void onMousePress(QMouseEvent*);

}; // </ActorEditTool>

#endif