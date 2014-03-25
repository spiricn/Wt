#ifndef WTE_SCRIPTER_H
#define WTE_SCRIPTER_H

#include "ui_Scripter.h"

#include "stdafx.h"

class Scripter : public QDialog{
Q_OBJECT;

public:
	Scripter();

	virtual ~Scripter();

private slots:

	void onOpen();

	void onSave();

	void onRun();

private:
	Ui::Scripter ui;
	
}; // </Scripter>


#endif // </WTE_SCRIPTER_H>
