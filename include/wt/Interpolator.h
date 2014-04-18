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
		eTYPE_LINEAR_FORWARD,
		eTYPE_LINEAR_BACKWARD,
		eTYPE_HALF_SINE,
	}; // </InterpolationType>

	Interpolator() : mFinished(true){
	}

	Interpolator(const T& start, const T& end, float duration, bool loop=false, InterpolationType type=eTYPE_LINEAR_FORWARD) : 
	mStart(start), mEnd(end), mDuration(duration), mTime(0), mLoop(loop), mType(type), mFinished(false), mValue(start){
	}

	float getPercentage() const{
		return mTime / mDuration;
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
		case eTYPE_LINEAR_FORWARD:
			a = a;
			break;

		case eTYPE_LINEAR_BACKWARD:
			a = 1.0f - a;
			break;

		case eTYPE_HALF_SINE:
			a = glm::sin( a * math::PI );

			break;
		}

		mValue = glm::mix(mStart, mEnd, a);

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

private:
	T mStart, mEnd;
	float mTime, mDuration;
	bool mLoop, mFinished;
	InterpolationType mType;
	T mValue;

}; // </Interpolator>

typedef Interpolator<float> FloatInterpolator;

typedef Interpolator<double> DoubleInterpolator;

typedef Interpolator<int32_t> IntInterpolator;

typedef Interpolator<glm::vec3> Vec3Interpolator;

typedef Interpolator<glm::vec2> Vec2Interpolator;

} // </wt>

#endif // </WT_INTERPOLATOR_H>
