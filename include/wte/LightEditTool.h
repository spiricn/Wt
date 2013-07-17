#ifndef WTE_LIGHTEDITTOOL_H
#define WTE_LIGHTEDITTOOL_H

#include <qdialog.h>

#include "ui_LightEditor.h"

#include "wte/ResourcePickerDialog.h"
#include "wte/SceneView.h"

class LightEditTool : public QDialog{
Q_OBJECT;

private:
	Ui::LightEditor ui;
	SceneView* mSceneView;

	wt::Scene* mScene;

public:
	LightEditTool(SceneView* scene, QWidget*);

private:

protected slots:
	void onAssetsLoaded();

	void onColorClicked();

	void onDiffuseIntensityChanged(int);

	void onAmbientIntensityChanged(int);

	void onSetDirection();

	void onColorChanged();

}; // </LightEditTOol>

#endif // </WTE_LIGHTEDITTOOL_H>