#include "stdafx.h"

#include "wte/HeightmapCreateDialog.h"

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

	uint32_t numRows = ui.numRows->value();

	uint32_t numCols = ui.numColumns->value();

	int16_t initialValue = ui.initialValue->value();

	std::ofstream outfile(path.toStdString().c_str(), std::ios::binary);

	for(int i=0; i<numRows*numCols; i++){
		outfile.write((char*)&initialValue, sizeof(int16_t));
	}


	mResult = path;

	outfile.close();

	close();
}
