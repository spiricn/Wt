#include "stdafx.h"

#include <Wt/Texture.h>

#include "wte/TextureManagerTab.h"

TextureManagerTab::TextureManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getTextureManager(), true){
	ui.setupUi(this);

	setAcceptDrops(true);

	setTreeWidget(ui.treeWidget);
}

void TextureManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.tga)"));

	LOG("SETTING IMAGE URI %s", path.toStdString().c_str());

	
	// Set uri relative to AResourceSystem RFS
	((wt::Texture2D*)item->userData)->setUri( getAssets()->getRelativeURI( path.toStdString() ) );
}

void TextureManagerTab::dropEvent(QDropEvent* evt){
	LOG("DROP %p", ui.treeWidget->itemAt(
		ui.treeWidget->mapFromGlobal(evt->pos())));
}