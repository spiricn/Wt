#include "stdafx.h"

#include <qcolordialog.h>

#include "wte/LightEditTool.h"

LightEditTool::LightEditTool(SceneView* scene, QWidget* parent) : QDialog(parent){
	setWindowFlags(Qt::Tool);

	ui.setupUi(this);

	mSceneView = scene;

	mScene = scene->getScene();

	ui.ambientIntensitySlider->setValue(30);
	onAmbientIntensityChanged(ui.ambientIntensitySlider->value());

	ui.diffuseItensitySlider->setValue(30);
	onDiffuseIntensityChanged(ui.diffuseItensitySlider->value());

	connect(parent, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));
}

void LightEditTool::onSetDirection(){
	mScene->getDirectionalLight().mDirection = mScene->getCamera().getForwardVec();
}

void LightEditTool::onAssetsLoaded(){
	ui.ambientIntensitySlider->setValue( (mScene->getDirectionalLight().mAmbientIntesity/2.0f) * 100);

	ui.diffuseItensitySlider->setValue( (mScene->getDirectionalLight().mDiffuseItensity/2.0f) * 100);
}

void LightEditTool::onColorClicked(){
	wt::DirectionalLight& l = mScene->getDirectionalLight();


	QColorDialog dlg(QColor(
		l.mColor.mBlue*255.0f,
		l.mColor.mGreen*255.0f,
		l.mColor.mBlue*255.0f
		), this);

	dlg.exec();


	QColor clr = dlg.selectedColor();
	l.mColor = wt::Color(
		clr.red()/255.0f, 
		clr.green()/255.0f, 
		clr.blue()/255.0f);
	mSceneView->update();
}

void LightEditTool::onDiffuseIntensityChanged(int val){
	wt::DirectionalLight& l = mScene->getDirectionalLight();
	l.mDiffuseItensity = (val/100.0f) * 2;
	mSceneView->update();
}

void LightEditTool::onAmbientIntensityChanged(int val){
	wt::DirectionalLight& l = mScene->getDirectionalLight();
	l.mAmbientIntesity = (val/100.0f) * 2;
	mSceneView->update();
}