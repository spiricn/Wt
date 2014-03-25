#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include "wt/ProcessManager.h"

#define TD_TRACE_TAG "AProcess"

namespace wt
{

AProcess::AProcess(const String& name) : mAlive(true), mNextProc(NULL), mManager(NULL),
	mAttached(false), mName(name), mPid(kINVALID_PID), mSuspended(false), mUsage(0.0f), mUsageTime(0.0f){
}

void AProcess::onProcessAttach(ProcessManager* manager, Pid pid){
	WT_ASSERT(!isAttached(), "Process already attached to a manager");

	mManager = manager;
	mAttached = true;
	mPid = pid;

	mTimer.reset();

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

void AProcess::suspend(bool state){
	mSuspended = state;
}

bool AProcess::isSuspended() const{
	return mSuspended;
}

float AProcess::getRunTime() const{
	return const_cast<AProcess*>(this)->mTimer.peekSeconds();
}

float AProcess::getUsage() const{
	return mUsage;
}

void AProcess::setUsage(float usage){
	mUsage = usage;
}

void AProcess::setUsageTime(float time){
	mUsageTime = time;
}

float AProcess::getUsageTime() const{
	return mUsageTime;
}

} // </wt>