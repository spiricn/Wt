#include "stdafx.h"

#include "wte/GodrayTool.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "GodrayTool"

GodrayTool::GodrayTool(QWidget* parent, AToolManager* manager) :
	QWidget(parent), mInitializing(true), ATool(manager){

    ui.setupUi(this);

	// Size
	ui.size->setValueRange(1, 100);

	// Decay
	ui.decay->setValueRange(0, 3);
	ui.decay->setSingleStep(0.1);

	// Weight
	ui.weight->setValueRange(0, 10);
	ui.weight->setSingleStep(0.1);

	// Density
	ui.density->setValueRange(0, 3);
	ui.density->setDecimals(3);
	ui.density->setSingleStep(0.01);

	// Exposure
	ui.exposure->setValueRange(0, 1);
	ui.exposure->setSingleStep(0.001);
	ui.exposure->setDecimals(3);

	// Sample number
	ui.sampleNumber->setValueRange(1, 300);
}

void GodrayTool::loadParams(){
	mInitializing = true;

	wt::Scene::GodRayParams params;

	WTE_CTX.getScene()->getGodRayParams(params);

	ui.enabled->setChecked( params.enabled );
	ui.size->setValue( params.sourceSize );
	ui.rayColor->setColor( params.rayColor );
	ui.sourceColor->setColor( params.sourceColor );
	ui.decay->setValue(params.decay);
	ui.exposure->setValue(params.exposure);
	ui.sampleNumber->setValue(params.sampleNumber);
	ui.weight->setValue( params.weight);
	ui.sourcePosition->setValue( params.sourcePosition);
	ui.sourceColor->setColor( params.sourceColor);
	ui.rayColor->setColor( params.rayColor);
	ui.density->setValue( params.density );

	mInitializing = false;
}

void GodrayTool::onSceneLoaded(){
	loadParams();
}

void GodrayTool::onParamChanged(){
	if(mInitializing){
		return;
	}

	wt::Scene::GodRayParams params;

	WTE_CTX.getScene()->getGodRayParams(params);

	params.enabled = ui.enabled->isChecked();
	params.decay = ui.decay->getValue();
	params.exposure = ui.exposure->getValue();
	params.sampleNumber = ui.sampleNumber->getValue();
	params.sourceSize = ui.size->getValue();
	params.rayColor = ui.rayColor->getColor();
	params.weight = ui.weight->getValue();
	params.density = ui.density->getValue();
	params.sourceColor = ui.sourceColor->getColor();
	params.sourceTexture = WTE_CTX.getAssets()->getTextureManager()->getFromPath( ui.sourceTexture->text().toStdString() );

	WTE_CTX.getScene()->setGodRayParams(params);
}

void GodrayTool::onReset(){
	wt::Scene::GodRayParams params;
	WTE_CTX.getScene()->setGodRayParams(params);

	loadParams();
}

void GodrayTool::onPickTexture(){
	wt::Texture2D* rsrc = ResourcePickerDialog::pickResource<wt::Texture2D>(this, WTE_CTX.getAssets()->getTextureManager(), "Pick godray source texture");
	if(rsrc){
		ui.sourceTexture->setText(rsrc->getPath().c_str());
		onParamChanged();
	}
}