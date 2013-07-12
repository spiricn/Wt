#include "stdafx.h"

#include "wte/ImageManagerTab.h"
#include "wte/ResourcePickerDialog.h"

ImageManagerTab::ImageManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets,
	assets->getImageManager(), true){
	ui.setupUi(this);

	setTreeWidget(ui.treeWidget);
}

void ImageManagerTab::onCtxMenuSetUri(RTWItem* item){
	// ask user for input & set image URI
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.tga)"));

	((wt::Image*)item->userData)->setUri(path.toStdString());
}