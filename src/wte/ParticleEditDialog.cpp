#include "stdafx.h"

#include "wte/ParticleEditDialog.h"
#include "wte/ResourcePickerDialog.h"

#define TD_TRACE_TAG "ParticleEditDialog"

ParticleEditDialog::ParticleEditDialog(QWidget* parent, wt::ParticleEffectResource* effect) : QDialog(parent), mEffect(NULL), mCurrentLayer(NULL), mLoadingLayer(false){
    ui.setupUi(this);

	ui.life->setMode(VecEditWidget::eMODE_VEC2);
	ui.size->setMode(VecEditWidget::eMODE_VEC2);

	loadEffect(effect);

	connect(ui.layerList, SIGNAL(onItemActivated(void*)),
		this, SLOT(onLayerActivated(void*)));
}

void ParticleEditDialog::loadEffect(wt::ParticleEffectResource* effect){
	mEffect = effect;

	ui.layerList->clearItems();

	if(!effect){
		mCurrentLayer = NULL;
		return;
	}

	for(wt::ParticleEffectResource::LayerMap::iterator iter=effect->getLayerMap().begin(); iter!=effect->getLayerMap().end(); iter++){
		ui.layerList->addItem(iter->first.c_str(), iter->second);
	}

	if(!effect->getLayerMap().empty()){
		loadLayer(effect->getLayerMap().begin()->second);
	}
}

void ParticleEditDialog::loadLayer(wt::ParticleLayerResource* layer){
	mLoadingLayer = true;

	mCurrentLayer = layer;

	const wt::ParticleLayerResource::LayerDesc& desc = layer->getDesc();

	ui.localVel->setValue(desc.localVelocity);
	ui.randomVel->setValue(desc.randomVelocity);
	ui.emissionRate->setValue(desc.emissionRate);
	ui.emissionVol->setValue(desc.emissionVolume);
	ui.life->setValue(glm::vec2(desc.minLife, desc.maxLife));
	ui.size->setValue(glm::vec2(desc.minSize, desc.maxSize));
	ui.particleNum->setValue(desc.particleNumber);
	ui.sizeGrow->setValue(desc.sizeGrow);
	if(desc.texture){
		ui.texture->setText(desc.texture->getPath().c_str());
	}
	ui.color1->setColor( desc.colorAnimation[0] );
	ui.color2->setColor( desc.colorAnimation[1] );
	ui.color3->setColor( desc.colorAnimation[2] );
	ui.color4->setColor( desc.colorAnimation[3] );

	mLoadingLayer = false;
}

void ParticleEditDialog::onParamChanged(){
	if(mLoadingLayer || !mCurrentLayer){
		return;
	}

	wt::ParticleLayerResource::LayerDesc& desc = mCurrentLayer->getDesc();

	ui.localVel->getValue(desc.localVelocity);
	ui.randomVel->getValue(desc.randomVelocity);
	ui.emissionVol->getValue(desc.emissionVolume);
	desc.emissionRate = ui.emissionRate->value();
	desc.minLife = ui.life->getX();
	desc.maxLife = ui.life->getY();
	desc.minSize = ui.size->getX();
	desc.maxSize = ui.size->getY();
	desc.sizeGrow = ui.sizeGrow->value();
	desc.texture = mEffect->getManager()->getResourceSystem()->getTextureManager()->getFromPath(ui.texture->text().toStdString());
	
	desc.colorAnimation[0] = ui.color1->getColor();
	desc.colorAnimation[1] = ui.color2->getColor();
	desc.colorAnimation[2] = ui.color3->getColor();
	desc.colorAnimation[3] = ui.color4->getColor();
}

void ParticleEditDialog::setEffect(wt::ParticleEffectResource* effect){
	loadEffect(effect);
}

void ParticleEditDialog::onNewLayer(){
	QString res = QInputDialog::getText(this, "New layer", "Layer name:");
	wt::ParticleLayerResource* layer = NULL;

	if(res.size()){
		wt::ParticleLayerResource::LayerDesc desc;
		ui.layerList->addItem(
			res, layer = mEffect->createLayer(res.toStdString(), desc)
		);
	}

	if(layer){
		loadLayer(layer);
	}
}

void ParticleEditDialog::onDeleteLayer(){
}

void ParticleEditDialog::onPickTexture(){
	wt::Texture2D* texture = ResourcePickerDialog::pickResource<wt::Texture2D>(
		this, mEffect->getManager()->getResourceSystem()->getTextureManager(), "Pick particle texture");
	if(texture){
		ui.texture->setText(texture->getPath().c_str());
	}
}

void ParticleEditDialog::onLayerActivated(void* item){
	wt::ParticleLayerResource* layer = static_cast<wt::ParticleLayerResource*>(item);

	loadLayer(layer);
}