#ifndef WT_PROCESS_H
#define WT_PROCESS_H

#include "wt/Exception.h"
#include "wt/Sp.h"

namespace wt{

class AProcess;
class ProcessManager;

typedef wt::Sp<AProcess> ProcPtr;

class AProcess{
friend class ProcessManager;

public:
	typedef Uint32 Pid;

private:
	Pid mPid;
	ProcessManager* mManager;
	bool mIsAlive;
	bool mIsAttached;
	ProcPtr mNextProc;

protected:

	void attach(ProcessManager* manager, Pid pid);

	void deattach();

public:
	AProcess();

	Pid getPid() const;

	bool isAttached() const;

	bool isAlive() const;

	ProcessManager* getManager();

	ProcPtr getNext();

	ProcPtr setNext(ProcPtr proc);

	void killProcess();

	/** called at every iteration of process manager update */
	virtual void onProcUpdate(float dt)=0;

	/** called when the process is attached to a process manager, before the first update */
	virtual void onProcStart();

	/** called when the process is deattached from a process manager, after the last update */
	virtual void onProcEnd();

	virtual ~AProcess();

}; // </AProcess>

}; // </wt>

#endif