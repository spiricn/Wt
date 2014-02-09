#include "wt/stdafx.h"
#include "wt/ConditionVar.h"

#define TD_TRACE_TAG "ConditionVar"

namespace wt
{

#if defined(WIN32)

ConditionVar::ConditionVar(){
	InitializeConditionVariable(&mWin32Handle);
}

ConditionVar::~ConditionVar(){
}

void ConditionVar::wait(Mutex& mutex){
	SleepConditionVariableCS(&mWin32Handle, &mutex.getWin32Handle(), INFINITE);
}

void ConditionVar::wakeOne(){
	WakeConditionVariable(&mWin32Handle);
}

void ConditionVar::wakeAll(){
	WakeAllConditionVariable(&mWin32Handle);
}

#elif defined(__linux__)

ConditionVar::ConditionVar(){
	pthread_cond_init(&mHandle, NULL);
}

ConditionVar::~ConditionVar(){
	pthread_cond_destroy(&mHandle);
}

void ConditionVar::wait(Mutex& mutex){
	pthread_cond_wait(&mHandle, mutex.getPthreadHandle());
}

void ConditionVar::wakeOne(){
	pthread_cond_signal(&mHandle);
}

void ConditionVar::wakeAll(){
	pthread_cond_broadcast(&mHandle);
}

#else
	#error Not implemented on this platform

#endif

} // </wt>