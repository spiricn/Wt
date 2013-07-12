#include "wt/stdafx.h"

#include "wt/Mutex.h"
#include "wt/Exception.h"
#include "wt/Timer.h"

namespace wt{

#if defined(WIN32)

Mutex::Mutex(){
	InitializeCriticalSection(&mWin32Handle);
}

Mutex::~Mutex(){
	DeleteCriticalSection(&mWin32Handle);
}

bool Mutex::tryLock(float timeout){
	Timer time;

	do{
		if(TryEnterCriticalSection(&mWin32Handle) != 0){
			return true;
		}
	}while(time.peekSeconds() < timeout);

	return false;
}

void Mutex::lock(){
	EnterCriticalSection(&mWin32Handle);
}

void Mutex::unlock(){
	LeaveCriticalSection(&mWin32Handle);
}
#else

	#error Not implemented on this platform

#endif // </WIN32>

}; // </wt>
