#include "stdafx.h"

#include "wte/HeightmapCreateDialog.h"

const char* HeightmapCreateDialog::TAG = "HeightmapCreateDialog";

HeightmapCreateDialog::HeightmapCreateDialog(QWidget* parent) : QDialog(parent){
    ui.setupUi(this);
}


void HeightmapCreateDialog::onCreate(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save heightmap");

	if(!path.size()){
		return;
	}

	wt::Uint32 numRows = ui.numRows->value();

	wt::Uint32 numCols = ui.numColumns->value();

	wt::Int16 initialValue = ui.initialValue->value();

	std::ofstream outfile(path.toStdString().c_str(), std::ios::binary);

	for(int i=0; i<numRows*numCols; i++){
		outfile.write((char*)&initialValue, sizeof(wt::Int16));
	}


	mResult = path;

	outfile.close();

	close();
}
