#ifndef WT_PROCESSMANAGER_H
#define WT_PROCESSMANAGER_H

#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include "wt/Timer.h"

namespace wt{

class ProcessManager{
public:
	typedef std::list<ProcPtr> ProcList;

	typedef ProcList::iterator ProcIterator;

public:
	ProcessManager();

	~ProcessManager();

	void update(float dt);
	
	ProcPtr attach(ProcPtr proc);

	ProcPtr findProcess(AProcess::Pid pid);

	ProcIterator getProcBeg();

	ProcIterator getProcEnd();

private:
	ProcList mProcesses;
	AProcess::Pid mPidCounter;

}; // </ProcessManager>

}; // </wt>

#endif