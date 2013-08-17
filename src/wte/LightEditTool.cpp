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
	
	light.color = ui.color->getColor();

	mScene->setDirectionalLight(light);
}

void LightEditTool::onSetDirection(){
	wt::DirectionalLight light;
	mScene->getDirectionalLight(light);
	
	light.direction = mScene->getCamera().getForwardVec();

	mScene->setDirectionalLight(light);
	
}

void LightEditTool::onAssetsLoaded(){
	wt::DirectionalLight light;
	mScene->getDirectionalLight(light);

	ui.ambientIntensitySlider->setValue( (light.ambientIntensity/2.0f) * 100);

	ui.diffuseItensitySlider->setValue( (light.diffuseIntensity/2.0f) * 100);
}

void LightEditTool::onColorClicked(){
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);

	QColorDialog dlg(QColor(
		l.color.blue*255.0f,
		l.color.green*255.0f,
		l.color.blue*255.0f
		), this);

	dlg.exec();

	QColor clr = dlg.selectedColor();
	l.color = wt::Color(
		clr.red()/255.0f, 
		clr.green()/255.0f, 
		clr.blue()/255.0f);

	mSceneView->update();
}

void LightEditTool::onDiffuseIntensityChanged(int val){
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);

	l.diffuseIntensity = (val/100.0f) * 2;
	mScene->setDirectionalLight(l);

	mSceneView->update();
}

void LightEditTool::onAmbientIntensityChanged(int val){	
	wt::DirectionalLight l;
	mScene->getDirectionalLight(l);
	l.ambientIntensity = (val/100.0f) * 2;
	mScene->setDirectionalLight(l);
	mSceneView->update();
}