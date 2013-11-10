#include "wt/stdafx.h"

#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "ProcessManager"

namespace wt{

void ProcessManager::update(float dt){
	ProcList::iterator procEnd = mProcesses.end();
	ProcList::iterator procIter = mProcesses.begin();

	while(procIter != procEnd){
		ProcPtr proc = *procIter;

		if(proc->isAlive()){
			proc->onProcUpdate(dt);
		}
		else{
			ProcPtr next = proc->getNext();

			if(next){
				attach(next);
			}

			procIter = mProcesses.erase(procIter);

			proc->onProcEnd();

			proc->detach();
		}

		if(procIter != mProcesses.end()){
			procIter++;
		}
	}
}

ProcessManager::~ProcessManager(){
	mProcesses.clear();
}
	
ProcPtr ProcessManager::attach(ProcPtr proc){
	WT_ASSERT(proc->getManager() == NULL, "Process already attached to a manager");

	// TODO should probably think of a better way of generating PIDs

	proc->onProcessAttach(this, mPidCounter++);

	mProcesses.push_back(proc);
	
	return proc;
}


}; // </wt>