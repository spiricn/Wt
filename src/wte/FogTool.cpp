#include "stdafx.h"

#include "wte/FogTool.h"

#define TD_TRACE_TAG "FogTool"

FogTool::FogTool(SceneView* sceneView, AToolManager* manager, QWidget* parent) : QDialog(parent), mSceneView(sceneView), ATool(manager){
    ui.setupUi(this);

	wt::FogDesc fog = mSceneView->getScene()->getFogDesc();

	ui.color->setColor(fog.color);
	ui.enabled->setChecked(fog.enabled);

	ui.density->setValueRange(0, 0.1);
	ui.density->setSingleStep(0.00001);
	ui.density->setValue(fog.density);
}

void FogTool::onColorChanged(){
	wt::FogDesc fog = mSceneView->getScene()->getFogDesc();

	fog.color = ui.color->getColor();

	mSceneView->getScene()->setFogDesc(fog);
}

void FogTool::onDensityChanged(){
	wt::FogDesc fog = mSceneView->getScene()->getFogDesc();

	fog.density = ui.density->getValue();

	mSceneView->getScene()->setFogDesc(fog);
}

void FogTool::onStateChanged(bool state){
	wt::FogDesc fog = mSceneView->getScene()->getFogDesc();

	fog.enabled = state;

	mSceneView->getScene()->setFogDesc(fog);
}
