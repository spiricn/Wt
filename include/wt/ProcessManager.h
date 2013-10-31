#ifndef WT_PROCESSMANAGER_H
#define WT_PROCESSMANAGER_H

#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include "wt/Timer.h"

namespace wt{

class ProcessManager{
private:
	typedef std::list<ProcPtr> ProcList;
	ProcList mProcesses;

public:
	~ProcessManager();

	void update(float dt);
	
	void attach(ProcPtr proc);

	void deattach(ProcPtr proc);

}; // </ProcessManager>

}; // </wt>

#endif