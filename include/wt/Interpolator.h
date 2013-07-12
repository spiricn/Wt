#ifndef WT_INTERPOLATOR_H
#define WT_INTERPOLATOR_H

#include "wt/ASoundSystem.h"
#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"

namespace wt{


template<class T>
class Interpolator{
public:
	enum InterpolationType{
		eLINEAR,
		eEASE_IN_QUAD
	};

private:
	T mStart, mEnd;
	float mTime, mDuration;
	bool mLoop, mFinished;
	InterpolationType mType;
	T mValue;

public:
	Interpolator() : mFinished(true){
	}

	Interpolator(const T& start, const T& end, float duration, bool loop=false, InterpolationType type=eLINEAR) : 
	mStart(start), mEnd(end), mDuration(duration), mTime(0), mLoop(loop), mType(type), mFinished(false), mValue(start){
	}

	static float linearTimeFunc(float time){
		return time;
	}

	static float quadTimeFunc(float time){
		return time*time;
	}

	const T& getValue() const{
		return mValue;
	}

	bool isFinished() const{
		return mFinished;
	}

	bool update(float dt){
		if(mFinished){
			return true;
		}

		mTime += dt;
		if(mTime > mDuration){
			mTime = mLoop ? fmod(mTime, mDuration) : mDuration;
			mFinished = mLoop ? false : true;
		}

		float a = mTime/mDuration;

		switch(mType){
		case eLINEAR: a = a; break;
		case eEASE_IN_QUAD: a = a*a; break;
		}

		mValue = glm::mix(mStart, mEnd,
			 a
			 );

		return mFinished;
	}

	bool update(float dt, T& res){
		if(!update(dt)){
			res = mValue;
			return false;
		}
		else{
			return true;
		}
		
	}

}; // </Interpolator>

}; // </wt>

#endif // </WT_INTERPOLATOR_H>
