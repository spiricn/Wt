#include "stdafx.h"

#include <wt/SFSound.h>

#include "wte/SoundManagerTab.h"

#define TD_TRACE_TAG "SoundManagerTab"

SoundManagerTab::SoundManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getSoundManager(), true){
    ui.setupUi(this);

	initialize<wt::ASoundBuffer>(ui.treeWidget);
}

void SoundManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open Sound"), "", tr("Vorbis files(*.ogg)"));

	((wt::ASoundBuffer*)item->userData)->setUri( getAssets()->getRelativeURI( path.toStdString() ) );
}