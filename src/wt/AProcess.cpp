#include "wt/stdafx.h"

#include "wt/AProcess.h"

namespace wt{


void AProcess::attach(ProcessManager* manager, Pid pid){
	WT_ASSERT(!isAttached(), "Process already attached to a manager");

	mManager = manager;
	mIsAttached = true;
	mPid = pid;

	onProcStart();
}

void AProcess::deattach(){
	WT_ASSERT(isAttached(), "Process not attached to a manager");

	mIsAttached = false;
	mManager = NULL;
	mPid = 0;

	onProcEnd();
}

AProcess::AProcess() : mIsAlive(true), mNextProc(NULL), mManager(NULL), mIsAttached(false){
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