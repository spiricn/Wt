#ifndef WTE_MATERIALEDITDIALOG_H
#define WTE_MATERIALEDITDIALOG_H

#include "ui_MaterialEditDialog.h"

#include <wt/Material.h>

class MaterialEditDialog : public QDialog{
Q_OBJECT

public:

	static void edit(QWidget* parent, wt::Material* material);

protected slots:

	void accept();

	void reject();

private:
	MaterialEditDialog(QWidget* parent, wt::Material* material);

    Ui::MaterialEditDialog ui;
	wt::Material* mMaterial;

}; // </MaterialEditDialog>

#endif // </WTE_MATERIALEDITDIALOG_H>