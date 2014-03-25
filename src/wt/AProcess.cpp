#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "AProcess"

namespace wt
{

AProcess::AProcess(const String& name) : mAlive(true), mNextProc(NULL), mManager(NULL), mAttached(false), mName(name), mPid(kINVALID_PID), mSusspended(false){
}

void AProcess::onProcessAttach(ProcessManager* manager, Pid pid){
	WT_ASSERT(!isAttached(), "Process already attached to a manager");

	mManager = manager;
	mAttached = true;
	mPid = pid;

	onProcStart();
}

void AProcess::onProcUpdate(float){
}

void AProcess::detach(){
	WT_ASSERT(isAttached(), "Process not attached to a manager");

	mAttached = false;
	mManager = NULL;
	mPid = 0;
}

AProcess::Pid AProcess::getPid() const{
	return mPid;
}

bool AProcess::isAttached() const{
	return mAttached;
}

bool AProcess::isAlive() const{
	return mAlive;
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
	WT_ASSERT(mAlive, "Attempting to kill a dead process");

	mAlive = false;
}

void AProcess::onProcStart(){
}


void AProcess::onProcEnd(){
}

AProcess::~AProcess(){
}

const String& AProcess::getName() const{
	return mName;
}

void AProcess::susspend(bool state){
	mSusspended = state;
}

bool AProcess::isSusspended() const{
	return mSusspended;
}

} // </wt>