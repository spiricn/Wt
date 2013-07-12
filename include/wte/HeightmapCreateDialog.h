#ifndef WTE_HEIGHTMAPCREATEDIALOG_H
#define WTE_HEIGHTMAPCREATEDIALOG_H

#include "ui_HeightmapCreateDialog.h"

class HeightmapCreateDialog : public QDialog{
Q_OBJECT

private:
    Ui::HeightmapCreateDialog ui;
	QString mResult;

public:
    HeightmapCreateDialog(QWidget* parent=NULL);

	static QString createHeightmap(QWidget* parent){
		HeightmapCreateDialog dlg(parent);

		dlg.exec();

		return dlg.mResult;
	}

protected slots:

	void onCreate();

}; // </HeightmapCreateDialog>

#endif // </WTE_HEIGHTMAPCREATEDIALOG_H>