#ifndef WTE_FOGTOOL_H
#define WTE_FOGTOOL_H

#include "ui_FogTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

class FogTool : public QDialog, public ATool{
Q_OBJECT

public:
    FogTool(SceneView*, AToolManager* manager, QWidget* parent);

	void onSceneLoaded();

protected slots:
	void onColorChanged();

	void onDensityChanged();

	void onStateChanged(bool);

private:
    Ui::FogTool ui;
	SceneView* mSceneView;
}; // </FogTool>

#endif