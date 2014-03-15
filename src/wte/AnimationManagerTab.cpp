#include "stdafx.h"

#include "wte/AnimationManagerTab.h"

AnimationManagerTab::AnimationManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getAnimationManager(), true){

	ui.setupUi(this);

	initialize<wt::Animation>(ui.treeWidget);
}

void AnimationManagerTab::onCtxMenuSetUri(RTWItem* item){
	// ask user for input & set image URI
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open animation"), "", tr("Animation files (*.wta)"));

	((wt::Animation*)item->userData)->setUri( getAssets()->getRelativeURI( path.toStdString() ) );
}