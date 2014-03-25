#ifndef WTE_PROCESSMANAGERTOOL_H
#define WTE_PROCESSMANAGERTOOL_H

#include "ui_ProcessManagerTool.h"

#include "wte/ATool.h"

#include "stdafx.h"

#include <wt/ProcessManager.h>

class ProcessManagerTool : public QWidget, public ATool{
Q_OBJECT;

public:
	ProcessManagerTool(QWidget* parent, AToolManager* manager);

	virtual ~ProcessManagerTool();

	
private slots:
	void onUpdate(float dt);

private:
	struct Process{
		wt::AProcess::Pid pid;
		QTreeWidgetItem* item;
	}; // </Process>

	typedef std::map<wt::AProcess::Pid, Process*> ProcMap;

private:
	Process* findProc(wt::AProcess::Pid pid);

private:
	ProcMap mProcesses;
	float mUpdateCounter;
	Ui::ProcessManagerTool ui;
	
}; // </ProcessManagerTool>


#endif // </WTE_PROCESSMANAGERTOOL_H>
