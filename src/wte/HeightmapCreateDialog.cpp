#include "stdafx.h"

#include <wt/Heightmap.h>

#include "wte/HeightmapCreateDialog.h"
#include "wte/WtEditorContext.h"


#define TD_TRACE_TAG "HeightmapCreateDialog"

HeightmapCreateDialog::HeightmapCreateDialog(QWidget* parent) : QDialog(parent){
    ui.setupUi(this);
}


void HeightmapCreateDialog::onCreate(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save heightmap");

	if(!path.size()){
		return;
	}

	wt::Heightmap heightmap;

	heightmap.create(ui.numRows->value(), ui.numColumns->value(), ui.initialValue->value());

	heightmap.setRowScale(ui.rowScale->value());

	heightmap.setHeightScale(ui.heightScale->value());

	heightmap.setColumnScale(ui.columnScale->value());

	WTE_CTX.getAssets()->getHeightmapManager()->getLoader()->save(path.toStdString(), &heightmap);

	mResult = path;

	close();
}
