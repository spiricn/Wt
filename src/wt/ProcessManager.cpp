#include "wt/stdafx.h"

#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "ProcessManager"

namespace wt{

void ProcessManager::update(float dt){
	ProcList::iterator procEnd = mProcesses.end();
	ProcList::iterator procIter = mProcesses.begin();

#if 0
	float totalTime=0.0f;
	std::map<AProcess::Pid, float> procTimes;
#endif

	while(procIter != procEnd){
		ProcPtr proc = *procIter;

		//LOG("%d %d", proc->isAlive(), proc->getPid());
		if(proc->isAlive()){
#if 0
			StopWatch procTime;

#endif
			proc->onProcUpdate(dt);

#if 0
			procTimes[proc->getPid()] = procTime.getSeconds();
			totalTime += procTimes[proc->getPid()];
#endif
		}
		else{
			ProcPtr next = proc->getNext();
			if(next){
				attach(next);
			}

			procIter = mProcesses.erase(procIter);
			deattach(proc);
		}


		if(procIter != mProcesses.end()){
			procIter++;
		}
	}

#if 0
	for(std::map<AProcess::Pid, float>::iterator i=procTimes.begin(); i!=procTimes.end(); i++){
		float p = i->second/totalTime;
		LOGI("ProcessManager", "PID: %d CPU: %.2f%% MS: %d", i->first, p*100.0f, (uint32_t)(i->second*1000.0));
	}
#endif
}

ProcessManager::~ProcessManager(){
	mProcesses.clear();
}
	
void ProcessManager::attach(ProcPtr proc){
	static AProcess::Pid pid=0;

	proc->attach(this, pid++);
	mProcesses.push_back(proc);
}

void ProcessManager::deattach(ProcPtr proc){
	proc->deattach();
}

}; // </wt>