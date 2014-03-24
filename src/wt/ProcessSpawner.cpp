#include "wt/stdafx.h"
#include "wt/ProcessSpawner.h"
#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "ProcessSpawner"

namespace wt
{

ProcessSpawner::ProcessSpawner(uint32_t numProcesses, ...){
	va_list args;

	va_start(args, numProcesses);

	for(uint32_t i=0; i<numProcesses; i++){
		AProcess* proc = va_arg(args, AProcess*);
		mProcesses.push_back(proc);
	}

	va_end(args);
}

ProcessSpawner::~ProcessSpawner(){
}

void ProcessSpawner::onProcStart(){
	for(ProcessList::iterator iter=mProcesses.begin(); iter!=mProcesses.end(); iter++){
		getManager()->attach(*iter);
	}
}

void ProcessSpawner::onProcUpdate(float){
	bool allDead = true;

	for(ProcessList::iterator iter=mProcesses.begin(); iter!=mProcesses.end(); iter++){
		if((*iter)->isAlive()){
			allDead = false;
			break;
		}
	}

	if(allDead){
		killProcess();
	}
}

void ProcessSpawner::onProcEnd(){
}
	
} // </wt>

