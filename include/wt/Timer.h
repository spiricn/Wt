#ifndef WT_TIMER_H
#define WT_TIMER_H

#include "wt/stdafx.h"

namespace wt{

class Timer{
public:
	Timer();

	void reset();

	float peekSeconds();

	float getSeconds();

#if defined(WIN32)
private:
	LARGE_INTEGER mCounterFrequency, mLastCount;
#else
#endif

}; // </Timer>


class FPSCalculator{
private:
	float mTime;
	float mFps;
	float mUpdateFrequency;
	Uint32 mNumFrames;
	static const int kMAX_VALUES = 10;
	float mFpsValues[kMAX_VALUES];
	Uint32 mCurrentValue;
	
public:
	FPSCalculator() : mTime(0.0f), mFps(0), mNumFrames(0), mUpdateFrequency(.3f), mCurrentValue(0){
		for(int i=0; i<kMAX_VALUES; i++){
			mFpsValues[i] = -1;
		}
	}

	float getFps() const{
		return mFps;
	}

	bool update(float dt){
		mTime += dt;
		++mNumFrames;

		if(mTime >= mUpdateFrequency){
			mFpsValues[mCurrentValue] = mNumFrames/mTime;
			
			Uint32 numValues=0;
			float totalFps=0;
			for(int i=0; i<kMAX_VALUES; i++){
				if(mFpsValues[i] >= 0.0f){
					++numValues;
					totalFps += mFpsValues[i];
				}
			}
			mFps = totalFps/numValues;
			
			mCurrentValue = (mCurrentValue+1)%kMAX_VALUES;
			
			mTime = 0.0f;
			mNumFrames = 0;
			
			return true;
		}
		return false;
	}

}; // </FPSCalculator>

}; // </wt>

#endif // </WT_TIMER_H>

