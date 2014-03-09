#ifndef WTE_ACTOREDITTOOL_H
#define WTE_ACTOREDITTOOL_H

#include "ui_ActorEditTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

#include <wte/ParticleEditDialog.h>

class ActorEditTool : public QDialog, public ATool{
Q_OBJECT

public:
	ActorEditTool(SceneView* sceneView, QWidget* parent, AToolManager*);

	void onToolLostFocus();

	void onToolGainFocus();

protected slots:
	void onEditModel();

	void onAttachBoneChanged(QString);

	void onActorVisibilityChange(bool);

	void onAttach();

	void onPickAttachActor();

	void onBeforeSceneUnload();

	void onNewActor();

	void onAnimationSelected(QString);

	void onMouseDrag(MouseDragEvent evt);

	void onToggleBrush();

	void onSelectActor();

	void onScaleChanged();

	void onTranslationChanged();

	void onEditParticleEffect();

	void onActorRename();

	void onRotationChanged();

	void onMousePress(QMouseEvent*);

	void onDeleteActor();

	void onBBoxGeometryChanged();

	void onEditActor();

	// Point light
	void onAmbientIntensityChanged();

	void onDiffuseIntensityChanged();

	void onLightColorChanged();

	void onAttenuationChanged();

	void onMinDistanceChanged();

	void onVolumeChanged();

	void onSoundAttenuationChanged();
	
	void onPitchChanged();


private:
	enum State{
		eSTATE_IVNALID = -1,

		eSTATE_NORMAL,
		eSTATE_PICK_ATTACH_ACTOR
	}; // </State>

	State mState;
    Ui::ActorEditTool ui;
	SceneView* mSceneView;
	wt::Scene* mScene;
	wt::Physics* mPhysics;
	wt::ModelledActor* mAttachActor;

	ParticleEditDialog* mParticleEditDialog;

	wt::ASceneActor* mSelectedActor;

	void selectActor(wt::ASceneActor* actor);
	
	void updateSelectionStats();

	bool mSelectingActor;

	void editActor();

}; // </ActorEditTool>

#endif