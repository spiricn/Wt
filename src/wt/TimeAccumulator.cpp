#include "wt/stdafx.h"

#include "wt/TimeAccumulator.h"

#define TD_TRACE_TAG "TimeAccumulator"

namespace wt{

TimeAccumulator::TimeAccumulator(float timeStep) : mTimeStep(timeStep), mTime(0.0f){
}

void TimeAccumulator::setStep(float step){
	mTimeStep = step;
}

float TimeAccumulator::getStep() const{
	return mTimeStep;
}

void TimeAccumulator::setTime(float time){
	mTime = time;
}

bool TimeAccumulator::update(float dt){
	mTime += dt;

	if(mTime > mTimeStep){
		mTime -= mTimeStep;
		return true;
	}
	else{
		return false;
	}
}


}; // </wt>