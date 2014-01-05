#ifndef WTE_MESHEDITDIALOG_H
#define WTE_MESHEDITDIALOG_H

#include "ui_MeshEditDialog.h"

#include <wt/AResourceSystem.h>
#include <wt/ModelSkin.h>

class MeshEditDialog : public QDialog{
Q_OBJECT

public:
	static bool edit(QWidget* widget, wt::AResourceSystem* assets, wt::ModelSkin::Mesh* mesh);

protected slots:

	void onEditTexture();

	void onEditMaterial();

	void onEditNormalMap();

	void accept();

private:
	MeshEditDialog(QWidget* parent, wt::AResourceSystem* assets, wt::ModelSkin::Mesh* mesh);

	wt::AResourceSystem* mAssets;
	wt::ModelSkin::Mesh* mMesh;
	Ui::MeshEditDialog ui;
	bool mAccepted;

}; // </MeshEditDialog>

#endif // </WTE_MESHEDITDIALOG_H>