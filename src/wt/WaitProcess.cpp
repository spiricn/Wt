#include "wt/stdafx.h"

#include "wt/WaitProcess.h"

namespace wt
{

WaitProcess::WaitProcess(float time) : mDuration(time), mCurrentTime(0.0f){
	WT_ASSERT(!glm::isnan(time) && time >= 0.0f, "Invalid wait time provided: %f", time);
}

void WaitProcess::onProcUpdate(float dt){
	mCurrentTime += dt;

	if(mCurrentTime >= mDuration){
		killProcess();
	}
}


} // </wt>
