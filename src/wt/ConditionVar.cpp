#include "wt/stdafx.h"

#include "wt/ConditionVar.h"


namespace wt{


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

#else

	#error Not implemented on this platform

#endif


}; // </wt>