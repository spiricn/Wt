#include "stdafx.h"

#include "wte/FogTool.h"

const char* FogTool::TAG = "FogTool";

FogTool::FogTool(SceneView* sceneView, QWidget* parent) : QDialog(parent), mSceneView(sceneView){
    ui.setupUi(this);
}
