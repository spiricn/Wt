#include "stdafx.h"

#include "wte/GodrayTool.h"
#include "wte/ResourcePickerDialog.h"

#define TD_TRACE_TAG "GodrayTool"

GodrayTool::GodrayTool(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets) : QWidget(parent), mScene(scene), mAssets(assets), mInitializing(true){
    ui.setupUi(this);

	ui.size->setValueRange(1, 100);

	wt::Scene::GodRayParams params;

	mScene->getGodRayParams(params);

	ui.enabled->setChecked( params.enabled );
	ui.size->setValue( params.sourceSize );
	ui.rayColor->setColor( params.rayColor );
	ui.sourceColor->setColor( params.sourceColor );

	mInitializing = false;
}

void GodrayTool::onParamChanged(){
	if(mInitializing){
		return;
	}

	wt::Scene::GodRayParams params;

	mScene->getGodRayParams(params);

	params.enabled = ui.enabled->isChecked();
	params.sourceSize = ui.size->getValue();
	params.rayColor = ui.rayColor->getColor();
	params.sourceColor = ui.sourceColor->getColor();
	params.sourceTexture = mAssets->getTextureManager()->getFromPath( ui.sourceTexture->text().toStdString() );

	mScene->setGodRayParams(params);
}

void GodrayTool::onPickTexture(){
	wt::Texture2D* rsrc = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager(), "Pick godray source texture");
	if(rsrc){
		ui.sourceTexture->setText(rsrc->getPath().c_str());
		onParamChanged();
	}
}