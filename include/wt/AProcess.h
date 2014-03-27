#ifndef WT_PROCESS_H
#define WT_PROCESS_H

#include "wt/Exception.h"
#include "wt/Sp.h"
#include "wt/Timer.h"

namespace wt{

class AProcess;
class ProcessManager;

typedef wt::Sp<AProcess> ProcPtr;

class AProcess{
public:
	typedef uint32_t Pid;

	static const int32_t kINVALID_PID = 0xFFFFFFFF;

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

	void suspend(bool state);

	bool isSuspended() const;

	virtual void onProcUpdate(float dt);

	virtual void onProcStart();

	virtual void onProcEnd();

	virtual ~AProcess();

	float getRunTime() const;

	float getUsage() const;

private:
	void onProcessAttach(ProcessManager* manager, Pid pid);

	void setUsage(float usage);

	void setUsageTime(float time);

	float getUsageTime() const;

private:
	Timer mTimer;
	float mUsage;
	float mUsageTime;
	Pid mPid;
	ProcessManager* mManager;
	ProcPtr mNextProc;
	String mName;
	bool mSuspended;
	bool mAlive;
	bool mAttached;
	
private:
	friend class ProcessManager;
}; // </AProcess>

}; // </wt>

#endif