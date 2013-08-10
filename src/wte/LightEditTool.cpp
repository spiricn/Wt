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

void LightEditTool::onColorChanged(){
	wt::DirectionalLight light;
	mScene->getDirectionalLight(light);
	
	light.mColor = ui.color->getColor();

	mScene->setDirectionalLight(light);
}

void LightEditTool::onSetDirection(){
	wt::DirectionalLight light;
	mScene->getDirectionalLight(light);
	
	light.mDirection = mScene->getCamera().getForwardVec();

	mScene->setDirectionalLight(light);
	
}

void LightEditTool::onAssetsLoaded(){
	wt::DirectionalLight light;
	mScene->getDirectionalLight(light);

	ui.ambientIntensitySlider->setValue( (light.mAmbientIntesity/2.0f) * 100);

	ui.diffuseItensitySlider->setValue( (light.mDiffuseItensity/2.0f) * 100);
}

void LightEditTool::onColorClicked(){
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);

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
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);

	l.mDiffuseItensity = (val/100.0f) * 2;
	mScene->setDirectionalLight(l);

	mSceneView->update();
}

void LightEditTool::onAmbientIntensityChanged(int val){	
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);
	l.mAmbientIntesity = (val/100.0f) * 2;
	mScene->setDirectionalLight(l);
	mSceneView->update();
}