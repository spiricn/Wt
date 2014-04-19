#include "stdafx.h"
#include "wte/HeightmapManagerTab.h"

#define TD_TRACE_TAG "HeightmapManagerTab"

HeightmapManagerTab::HeightmapManagerTab(QWidget* parent, wt::AResourceSystem* assets) : ARsrcManagerTab(parent,  assets, static_cast<void*>(assets->getHeightmapManager()), true){
	ui.setupUi(this);

	initialize<wt::Heightmap>(ui.treeWidget);
}

void HeightmapManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open heightmap"), "", tr("Wt heightmap files(*)"));

	static_cast<wt::Heightmap*>(item->userData)->setUri( WTE_CTX.getAssets()->getRelativeURI( path.toStdString() ) );
}