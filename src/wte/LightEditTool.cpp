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
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();

	desc.color = ui.color->getColor();

	mScene->setDirectionalLightDesc(desc);
}

void LightEditTool::onSetDirection(){
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();
	
	desc.direction = mScene->getCamera().getForwardVec();

	mScene->setDirectionalLightDesc(desc);
}

void LightEditTool::onAssetsLoaded(){
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();

	ui.ambientIntensitySlider->setValue( (desc.ambientIntensity/2.0f) * 100);

	ui.diffuseItensitySlider->setValue( (desc.diffuseIntensity/2.0f) * 100);

	ui.color->setColor( desc.color );
}

void LightEditTool::onColorClicked(){
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();

	QColorDialog dlg(QColor(
		desc.color.blue*255.0f,
		desc.color.green*255.0f,
		desc.color.blue*255.0f
		), this);

	dlg.exec();

	QColor clr = dlg.selectedColor();
	desc.color = wt::Color(
		clr.red()/255.0f, 
		clr.green()/255.0f, 
		clr.blue()/255.0f);

	mScene->setDirectionalLightDesc(desc);

	mSceneView->update();
}

void LightEditTool::onDiffuseIntensityChanged(int val){
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();

	desc.diffuseIntensity = (val/100.0f) * 2;

	mScene->setDirectionalLightDesc(desc);

	mSceneView->update();
}

void LightEditTool::onAmbientIntensityChanged(int val){	
	wt::DirectionalLight::Desc desc = mScene->getDirectionalLight()->getDesc();

	desc.ambientIntensity = (val/100.0f) * 2;

	mScene->setDirectionalLightDesc(desc);

	mSceneView->update();
}