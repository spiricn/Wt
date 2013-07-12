#ifndef WTE_FOGTOOL_H
#define WTE_FOGTOOL_H

#include "ui_FogTool.h"

#include "wte/SceneView.h"

class FogTool : public QDialog{
Q_OBJECT

private:
    Ui::FogTool ui;
	SceneView* mSceneView;

public:
    FogTool(SceneView*, QWidget* parent);

protected slots:

}; // </FogTool>

#endif