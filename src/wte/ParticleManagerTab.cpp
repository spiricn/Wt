#include "stdafx.h"

#include "wte/ParticleManagerTab.h"
#include "wte/ParticleEditDialog.h"

#define TD_TRACE_TAG "ParticleManagerTab"

ParticleManagerTab::ParticleManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets, assets->getParticleResourceManager(), false){
    ui.setupUi(this);

	initialize<wt::ParticleEffectResource>(ui.treeWidget);
}

void ParticleManagerTab::onTreeItemActivated(RTWItem* item){
	if(item->getType() == RTWItem::RESOURCE){
		wt::ParticleEffectResource* effect = (wt::ParticleEffectResource*)item->userData;
		
		ParticleEditDialog dlg(this, effect);

		dlg.exec();
	}
}
