#include "wt/stdafx.h"
#include "wt/Mutex.h"

namespace wt
{

#if defined(WIN32)

Mutex::Mutex(){
	InitializeCriticalSection(&mWin32Handle);
}

Mutex::~Mutex(){
	DeleteCriticalSection(&mWin32Handle);
}

void Mutex::lock(){
	EnterCriticalSection(&mWin32Handle);
}

void Mutex::unlock(){
	LeaveCriticalSection(&mWin32Handle);
}

#elif defined(__linux__)

Mutex::Mutex(){
	pthread_mutex_init(&mMutex, NULL);
}

Mutex::~Mutex(){
	pthread_mutex_destroy(&mMutex);
}

void Mutex::lock(){
	pthread_mutex_lock(&mMutex);
}

void Mutex::unlock(){
	pthread_mutex_unlock(&mMutex);
}

#else

	#error Not implemented on this platform

#endif // </WIN32>

} // </wt>
