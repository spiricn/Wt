#ifndef WT_PROCESS_H
#define WT_PROCESS_H

#include "wt/Exception.h"
#include "wt/Sp.h"

namespace wt{

class AProcess;
class ProcessManager;

typedef wt::Sp<AProcess> ProcPtr;

class AProcess{
public:
	typedef int32_t Pid;

	static const int32_t kINVALID_PID = -1;

public:
	AProcess(const String& name="");

	Pid getPid() const;

	bool isAttached() const;

	bool isAlive() const;

	const String& getName() const;

	ProcessManager* getManager();

	ProcPtr getNext();

	ProcPtr setNext(ProcPtr proc);

	void killProcess();

	void detach();

	void susspend(bool state);

	bool isSusspended() const;

	virtual void onProcUpdate(float dt);

	virtual void onProcStart();

	virtual void onProcEnd();

	virtual ~AProcess();

private:
	void onProcessAttach(ProcessManager* manager, Pid pid);

private:
	Pid mPid;
	ProcessManager* mManager;
	ProcPtr mNextProc;
	String mName;
	bool mSusspended;
	bool mAlive;
	bool mAttached;
	
private:
	friend class ProcessManager;
}; // </AProcess>

}; // </wt>

#endif