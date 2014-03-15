#include "stdafx.h"

#include <qstringlist.h>

#include "wte/SkyBoxManagerTab.h"
#include "wte/SkyboxEditDialog.h"


SkyBoxManagerTab::SkyBoxManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getSkyBoxManager()){

	ui.setupUi(this);

	initialize<wt::SkyBox>(ui.treeWidget);
}

void SkyBoxManagerTab::onTreeItemActivated(RTWItem* item){
	if(item->type == RTWItem::RESOURCE){
		wt::SkyBox* sky = (wt::SkyBox*)item->userData;
		SkyboxEditDialog::EditResult res = SkyboxEditDialog::createSkyBox(this, getAssets(), sky);
		if(res.ok){
			sky->setImages(
				const_cast<wt::Image*>(res.posX),
				const_cast<wt::Image*>(res.negX),
				const_cast<wt::Image*>(res.posY),
				const_cast<wt::Image*>(res.negY),
				const_cast<wt::Image*>(res.posZ),
				const_cast<wt::Image*>(res.negZ)
			);
		}
	}
}
