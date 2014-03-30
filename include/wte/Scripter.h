#ifndef WTE_SCRIPTER_H
#define WTE_SCRIPTER_H

#include "ui_Scripter.h"

#include <wt/ScriptResource.h>

#include "stdafx.h"

class Scripter : public QDialog{
Q_OBJECT;
public:
	Scripter();

	void open(wt::ScriptResource* rsrc);

private slots:

	void onReload();

	void onSave();

	void onRun();

private:
	void reload();

private:
	Ui::Scripter ui;

	wt::ScriptResource* mScript;
	
}; // </Scripter>


#endif // </WTE_SCRIPTER_H>
