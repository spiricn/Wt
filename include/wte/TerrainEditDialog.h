#ifndef WTE_TERRAINEDITDIALOG_H
#define WTE_TERRAINEDITDIALOG_H

#include "ui_TerrainEditDialog.h"

#include <wt/Terrain.h>
#include <wt/Assets.h>

class TerrainEditDialog : public QDialog{
Q_OBJECT

private:
    Ui::TerrainEditDialog ui;

	wt::TerrainDesc mResult;

	wt::Assets* mAssets;

	bool mAccepted;

public:
    TerrainEditDialog(QWidget* parent, wt::Assets* assets);

	static bool editTerrain(QWidget* parent, wt::Assets* assets, wt::TerrainDesc& res){
		TerrainEditDialog dlg(parent, assets);

		dlg.exec();

		if(dlg.mAccepted){
			res = dlg.mResult;
		}

		return dlg.mAccepted;
	}

protected slots:

	void onSave();

	void onTex1Pick();

	void onTex2Pick();

	void onTex3Pick();

	void onTexMapPick();

	void onHeightmapPick();

	void onNewHeightmap();
}; // </TerrainEditDialog>

#endif // </WTE_TERRAINEDITDIALOG_H>