#include "wt/stdafx.h"
#include "wt/ScopedLock.h"

#define TD_TRACE_TAG "ScopedLock"

namespace wt
{

ScopedLock::ScopedLock(Mutex& mutex) : mMutex(mutex){
	mMutex.lock();
}

ScopedLock::~ScopedLock(){
	mMutex.unlock();
}
	
} // </wt>

