#include "wt/stdafx.h"

#include "wt/Timer.h"

#define TD_TRACE_TAG "Timer"

namespace wt{

#if defined(WIN32)

Timer::Timer(){
	QueryPerformanceFrequency(&mCounterFrequency);
	QueryPerformanceCounter(&mLastCount);
}

void Timer::reset() {
	QueryPerformanceCounter(&mLastCount);
}

float Timer::peekSeconds(){
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	return float((current.QuadPart - mLastCount.QuadPart) / double(mCounterFrequency.QuadPart));
}

float Timer::getSeconds(){
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	float seconds = float((current.QuadPart - mLastCount.QuadPart) / double(mCounterFrequency.QuadPart));
	mLastCount = current;

	return seconds;
}

#else

	#error Not implemented on this platform

#endif

}; // </wt>