#include "stdafx.h"

#include <Wt/Texture.h>

#include "wte/TextureManagerTab.h"

TextureManagerTab::TextureManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getTextureManager(), true){
	ui.setupUi(this);

	setAcceptDrops(true);
}

void TextureManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.tga)"));

	LOG("SETTING IMAGE URI %s", path.toStdString().c_str());
	((wt::Texture2D*)item->userData)->setUri(path.toStdString());
}

void TextureManagerTab::dropEvent(QDropEvent* evt){
	LOG("DROP %p", ui.treeWidget->itemAt(
		ui.treeWidget->mapFromGlobal(evt->pos())));
}