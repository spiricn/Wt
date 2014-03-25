#include "wt/stdafx.h"

#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "ProcessManager"

namespace wt{

void ProcessManager::update(float dt){
	ProcList::iterator procEnd = mProcesses.end();
	ProcList::iterator procIter = mProcesses.begin();

	while(procIter != procEnd){
		ProcPtr proc = *procIter;

		if(proc->isAlive() && !proc->isSusspended()){
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

ProcPtr ProcessManager::findProcess(AProcess::Pid pid){
	WT_ASSERT(pid != AProcess::kINVALID_PID, "Invalid process pid %d", pid);

	for(ProcList::iterator iter=mProcesses.begin(); iter!=mProcesses.end(); iter++){
		if((*iter)->getPid() == pid){
			return *iter;
		}
	}

	return NULL;
}

ProcessManager::ProcIterator ProcessManager::getProcBeg(){
	return mProcesses.begin();
}

ProcessManager::ProcIterator ProcessManager::getProcEnd(){
	return mProcesses.end();
}

} // </wt>