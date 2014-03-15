#include "stdafx.h"

#include <Wt/Texture.h>

#include "wte/TextureManagerTab.h"

TextureManagerTab::TextureManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getTextureManager(), true){

	ui.setupUi(this);

	initialize<wt::Texture2D>(ui.treeWidget);
}

void TextureManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.tga)"));

	// Set uri relative to AResourceSystem RFS
	((wt::Texture2D*)item->userData)->setUri( getAssets()->getRelativeURI( path.toStdString() ) );
}
