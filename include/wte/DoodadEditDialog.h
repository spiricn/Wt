#ifndef WTE_DOODADEDITDIALOG_H
#define WTE_DOODADEDITDIALOG_H

#include "ui_DoodadEditDialog.h"

#include <wt/Model.h>


class DoodadEditDialog : public QDialog{
Q_OBJECT

public:
	struct EditResult{
		bool ok;
		wt::Model* model;
		QString skin;

		EditResult() : ok(false), model(NULL){
		}
	};

private:
    Ui::DoodadEditDialog ui;

	EditResult mResult;

	void setModel(wt::Model* model);

public:
    DoodadEditDialog(QWidget* parent=NULL);


	static EditResult edit(QWidget* parent){
		DoodadEditDialog dlg(parent);

		dlg.exec();

		return dlg.mResult;
	}
	

protected slots:
	void onPickModel();

	void onOk();
}; // </DoodadEditDialog>

#endif // </WTE_DOODADEDITDIALOG_H>