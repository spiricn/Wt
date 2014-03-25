#include "stdafx.h"
#include "wte/ProcessManagerTool.h"

#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "ProcessManagerTool"

ProcessManagerTool::ProcessManagerTool(QWidget* parent, AToolManager* manager) : QWidget(parent), ATool(manager), mUpdateCounter(0.0f){
	ui.setupUi(this);

	connect(&WTE_CTX, SIGNAL(update(float)),
		this, SLOT(onUpdate(float)));
}

ProcessManagerTool::Process* ProcessManagerTool::findProc(wt::AProcess::Pid pid){
	for(ProcMap::iterator iter = mProcesses.begin(); iter!=mProcesses.end(); iter++){
		if(iter->second->pid == pid){
			return iter->second;
		}
	}

	return NULL;
}

void ProcessManagerTool::onUpdate(float dt){
	mUpdateCounter += dt;
	if(mUpdateCounter >= 1.0f){
		mUpdateCounter = 0.0f;
	}
	else{
		return;
	}

	// Check for stopped processes
	for(ProcMap::iterator iter=mProcesses.begin(); iter!=mProcesses.end(); iter++){
		wt::ProcPtr proc = WTE_CTX.getProcManager()->findProcess(iter->second->pid);

		if(!proc){
			delete iter->second->item;
			delete iter->second;

			iter = mProcesses.erase(iter);
		}

		if(iter == mProcesses.end()){
			break;
		}
	}


	for(wt::ProcessManager::ProcIterator iter=WTE_CTX.getProcManager()->getProcBeg(); iter!=WTE_CTX.getProcManager()->getProcEnd(); iter++){
		wt::AProcess* wtProc = *iter;

		Process* proc = findProc((*iter)->getPid());

		if(proc == NULL){
			// Create new process item
			proc = new Process;

			proc->pid = wtProc->getPid();

			mProcesses.insert(std::make_pair(proc->pid, proc));

			proc->item = new QTreeWidgetItem(ui.procTree);

			ui.procTree->addTopLevelItem(proc->item);
		}

		// Update existing process
		proc->item->setText(0, QString("%1").arg(static_cast<int>(wtProc->getPid())));
		proc->item->setText(1, QString(wtProc->getName().c_str()));
		proc->item->setText(2, QString("%1 s").arg( static_cast<int>(wtProc->getRunTime())) );
		proc->item->setText(3, QString(wtProc->isSuspended() ? "Susspended" : "Active") );
		proc->item->setText(4, QString("%1 %").arg(wtProc->getUsage()*100.0f));
	}
}

ProcessManagerTool::~ProcessManagerTool(){
}
