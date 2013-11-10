#include "wt/stdafx.h"

#include "wt/AProcess.h"

#define TD_TRACE_TAG "AProcess"

namespace wt
{

AProcess::AProcess() : mIsAlive(true), mNextProc(NULL), mManager(NULL), mIsAttached(false){
}

void AProcess::onProcessAttach(ProcessManager* manager, Pid pid){
	WT_ASSERT(!isAttached(), "Process already attached to a manager");

	mManager = manager;
	mIsAttached = true;
	mPid = pid;

	onProcStart();
}

void AProcess::detach(){
	WT_ASSERT(isAttached(), "Process not attached to a manager");

	mIsAttached = false;
	mManager = NULL;
	mPid = 0;
}

AProcess::Pid AProcess::getPid() const{
	return mPid;
}

bool AProcess::isAttached() const{
	return mIsAttached;
}

bool AProcess::isAlive() const{
	return mIsAlive;
}

ProcessManager* AProcess::getManager(){
	return mManager;
}

ProcPtr AProcess::getNext(){
	return mNextProc;
}

ProcPtr AProcess::setNext(ProcPtr proc){
	mNextProc = proc;

	return proc;
}

void AProcess::killProcess(){
	WT_ASSERT(mIsAlive, "Attempting to kill a dead process");

	mIsAlive = false;
}

void AProcess::onProcStart(){
}


void AProcess::onProcEnd(){
}

AProcess::~AProcess(){
}

}; // </wt>