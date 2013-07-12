#include "stdafx.h"

#include "wte/FogTool.h"

#define TD_TRACE_TAG "FogTool"

FogTool::FogTool(SceneView* sceneView, QWidget* parent) : QDialog(parent), mSceneView(sceneView){
    ui.setupUi(this);
}
