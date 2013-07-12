#include "wt/stdafx.h"

#include "wt/Thread.h"

#define TD_TRACE_TAG "Thread"

namespace wt{


#if defined(WIN32)

Thread::Thread() : mWin32Id(-1), mWin32Handle(INVALID_HANDLE_VALUE), mRunning(false), mFinished(false){
}

Thread::~Thread(){
	if(mWin32Handle != INVALID_HANDLE_VALUE){
		CloseHandle(mWin32Handle);
	}
}



DWORD WINAPI Thread::startThread(LPVOID param){
	Thread* thread = static_cast<Thread*>(param);

	thread->mRunning = true;

	thread->run();

	thread->mFinished = true;
	thread->mRunning = false;

	return 0;
}

void Thread::start(){
	mRunning = true;
	mWin32Handle = CreateThread(
		NULL,
		0,
		&Thread::startThread,
		(LPVOID*)this,
		0,
		&mWin32Id);
}

bool Thread::wait(float sec){
	DWORD msec = sec<0 ? INFINITE : (DWORD)(sec*1000.0f);

	DWORD res = WaitForSingleObject(mWin32Handle, msec);

	return res == WAIT_OBJECT_0;
}

void Thread::sleep(float sec){
	Sleep( sec*1000.0f );
}

#else

	#error Not implemented on this platform

#endif

bool Thread::isRunning() const{
	return mRunning;
}

bool Thread::isFinished() const{
	return mFinished;
}

}; // </wt>