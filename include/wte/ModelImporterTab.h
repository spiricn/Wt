#ifndef WTE_MODELIMPORTERTAB_H
#define WTE_MODELIMPORTERTAB_H

#include "stdafx.h"

namespace wt{

class Assets;

}; // </wt>


#include <wt/AResourceSystem.h>

#include "ui_ModelImporterTab.h"

#include "wte/utils.h"

class ModelImporterTab : public QWidget{
	Q_OBJECT;

private:
	wt::AResourceSystem* mAssets;
	Ui::ModelImporter ui;

	bool sameFile(const QString& file1, const QString& file2);

protected:
	void dropEvent(QDropEvent* evt);

	void dragEnterEvent(QDragEnterEvent* evt);

	void dragMoveEvent(QDragMoveEvent* evt);

	void import(const QString& path);

public:
	ModelImporterTab(QWidget* parent, wt::AResourceSystem* assets);

protected slots:
	void onBatchConvert();

}; // </ModelImporterTab>

#endif // </WTE_MODELIMPORTERTAB_H>