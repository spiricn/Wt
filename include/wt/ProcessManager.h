#ifndef WT_PROCESSMANAGER_H
#define WT_PROCESSMANAGER_H

#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include "wt/Timer.h"

namespace wt{

class ProcessManager{
public:
	~ProcessManager();

	void update(float dt);
	
	ProcPtr attach(ProcPtr proc);

private:
	typedef std::list<ProcPtr> ProcList;

	ProcList mProcesses;
	AProcess::Pid mPidCounter;

}; // </ProcessManager>

}; // </wt>

#endif