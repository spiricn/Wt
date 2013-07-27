#include "stdafx.h"

#include "wte/ParticleManagerTab.h"
#include "wte/ParticleEditDialog.h"

#define TD_TRACE_TAG "ParticleManagerTab"

ParticleManagerTab::ParticleManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets, assets->getParticleResourceManager(), false){
    ui.setupUi(this);

	setTreeWidget(ui.treeWidget);
}

void ParticleManagerTab::destroyAll(){
	ARsrcManagerTab::destroyAll<wt::ParticleEffectResource>();
}

void ParticleManagerTab::refreshAll(){
	ui.treeWidget->buildTree<wt::ParticleEffectResource>(NULL, getAssets()->getParticleResourceManager());
}

void ParticleManagerTab::onCtxMenuCreateResource(RTWItem* item){
	createResource<wt::ParticleEffectResource>(item);
}

void ParticleManagerTab::onTreeItemActivated(RTWItem* item){
	if(item->getType() == RTWItem::RESOURCE){
		wt::ParticleEffectResource* effect = (wt::ParticleEffectResource*)item->userData;
		
		ParticleEditDialog dlg(this, effect);

		dlg.exec();
	}
}
