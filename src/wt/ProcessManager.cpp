#include "wt/stdafx.h"

#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "ProcessManager"

namespace wt
{

ProcessManager::ProcessManager() : mPidCounter(0){
}

void ProcessManager::update(float dt){
	ProcList::iterator procEnd = mProcesses.end();
	ProcList::iterator procIter = mProcesses.begin();

	float totalTime = 0.0f;

	while(procIter != procEnd){
		ProcPtr proc = *procIter;

		Timer procTime;

		if(proc->isAlive() && !proc->isSuspended()){
			procTime.reset();

			proc->onProcUpdate(dt);

			proc->setUsageTime(procTime.getSeconds());

			totalTime += proc->getUsageTime();
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

	for(ProcList::iterator iter=mProcesses.begin(); iter!=mProcesses.end(); iter++){
		(*iter)->setUsage( (((*iter)->getUsageTime())/totalTime) );
	}
}

ProcessManager::~ProcessManager(){
	mProcesses.clear();
}
	
ProcPtr ProcessManager::attach(ProcPtr proc){
	WT_ASSERT(proc->getManager() == NULL, "Process already attached to a manager");

	for(;;){
		++mPidCounter;

		if(mPidCounter == AProcess::kINVALID_PID){
			mPidCounter = 0;
		}

		if(!findProcess(mPidCounter)){
			break;
		}
	}

	proc->onProcessAttach(this, mPidCounter);

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