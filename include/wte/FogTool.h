#ifndef WTE_FOGTOOL_H
#define WTE_FOGTOOL_H

#include "ui_FogTool.h"

#include "wte/SceneView.h"
#include "wte/ATool.h"

class FogTool : public QDialog, public ATool{
Q_OBJECT

private:
    Ui::FogTool ui;
	SceneView* mSceneView;

public:
    FogTool(SceneView*, AToolManager* manager, QWidget* parent);

protected slots:

}; // </FogTool>

#endif