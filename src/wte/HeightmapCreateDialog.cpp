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
		"Save heightmap", "", "Heightmap files (*.wth)");

	if(!path.size()){
		return;
	}

	wt::Heightmap heightmap;

	heightmap.create(ui.numRows->value(), ui.numColumns->value(), ui.initialValue->value());

	heightmap.setRowScale(ui.rowScale->value());

	heightmap.setHeightScale(ui.heightScale->value());

	heightmap.setColumnScale(ui.columnScale->value());

	// We have to open the stream ourselves since the resource is not managed (thus has no file system associated with it)
	wt::StreamPtr stream = WTE_CTX.getAssets()->getFileSystem()->open(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	WTE_CTX.getAssets()->getHeightmapManager()->getLoader()->save(stream, &heightmap);

	mResult = path;

	close();
}
