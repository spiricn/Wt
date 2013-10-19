#include "stdafx.h"

#include "wte/FogTool.h"

#define TD_TRACE_TAG "FogTool"

FogTool::FogTool(SceneView* sceneView, AToolManager* manager, QWidget* parent) : QDialog(parent), mSceneView(sceneView), ATool(manager){
    ui.setupUi(this);
}
