#include "wt/stdafx.h"

#include "wt/NodeAnimation.h"
#include "wt/math.h"

#define SPLINE_TENSION 0.8f

namespace wt{
	
void NodeAnimation::calcTangent(Int32 k, glm::vec3& res) const{
	const PositionKey& prev = mPosKeys[ (k-1)<0?mPosKeys.size()-1:k-1 ];
	const PositionKey& next = mPosKeys[ (k+1)%mPosKeys.size() ];

	res = SPLINE_TENSION * (next.position - prev.position);
}

void NodeAnimation::setDuration(float dur){
	mDuration = dur;
}

float NodeAnimation::getDuration() const{
	return mDuration;
}

int NodeAnimation::findPosKey(float time) const{
	if(mPosKeys.size()==0){
		return -1;
	}

	for(Uint32 i=0; i<mPosKeys.size()-1; i++){
		if(time < mPosKeys[i+1].time){
			return i;
		}
	}

	return mPosKeys.size()-1;
}

int NodeAnimation::findRotKey(float time) const{
	if(mRotKeys.empty()){
		return -1;
	}

	for(Uint32 i=0; i<mRotKeys.size()-1; i++){
		if(time < mRotKeys[i+1].time){
			return i;
		}
	}

	return mRotKeys.size()-1;
}

int NodeAnimation::findScaleKey(float time) const{
	if(mScaleKeys.empty()){
		return -1;
	}

	for(Uint32 i=0; i<mScaleKeys.size()-1; i++){
		if(time < mScaleKeys[i+1].time){
			return i;
		}
	}

	return mScaleKeys.size()-1;
}

NodeAnimation::NodeAnimation() : mNodeName(""), mDuration(0.0f), mUseSplines(true){
}

void NodeAnimation::setTargetNode(const String& target){
	mNodeName = target;
}

const String& NodeAnimation::getTargetNode(){
	return mNodeName;
}

NodeAnimation::RotKeyList& NodeAnimation::getRotationKeys(){
	return mRotKeys;
}

NodeAnimation::PosKeyList& NodeAnimation::getPositionKeys(){
	return mPosKeys;
}

NodeAnimation::ScaleKeyList& NodeAnimation::getScaleKeys(){
	return mScaleKeys;
}

void NodeAnimation::evaluate(float time, glm::vec3& translation,
	glm::quat& rotation, glm::vec3& scale) const{
	/* Translation */
	int posKey = findPosKey(time);
	if(posKey != -1){

		const PositionKey& k1 = mPosKeys[posKey];

		const PositionKey& k2 = mPosKeys[ (posKey+1) % mPosKeys.size() ];

		float factor;
		float timeDiff;

		if(k2.time < k1.time){
			//factor = 
			timeDiff = mDuration-k1.time /* time until the end of the animation */+
				+ k2.time /* time until the beggining of the first (not always first?) frame */;
			if(time >= k1.time && time <= mDuration){
				factor = (time-k1.time)/timeDiff;
			}
			else{
				factor = (timeDiff-time)/timeDiff;
			}
		}
		else{
			timeDiff = k2.time-k1.time;
			factor = (time-k1.time)/timeDiff;
		}

		if(mUseSplines && 0){
			if(timeDiff == 0.0f){
				translation = k1.position;
			}
			else{
				glm::vec3 m1, m2;
				calcTangent(posKey, m1);
				calcTangent((posKey+1)%mPosKeys.size(), m2);

		

				translation.x = math::hermite(k1.position.x, k2.position.x, m1.x, m2.x, factor);
				translation.y = math::hermite(k1.position.y, k2.position.y, m1.y, m2.y, factor);
				translation.z = math::hermite(k1.position.z, k2.position.z, m1.z, m2.z, factor);
			}
		}
		else{
			if(timeDiff == 0.0f){
				translation = k1.position;
			}
			else{
				translation = glm::mix(k1.position, k2.position, factor);
			}
		}

	}
	else if(mPosKeys.size()==1){
		translation = mPosKeys[0].position;
	}

	/* Rotation */
	int rotKey = findRotKey(time);
	if(rotKey != -1){
		const RotationKey& k1 = mRotKeys[rotKey];

		const RotationKey& k2 = mRotKeys[ (rotKey+1)%mRotKeys.size() ];

		
		float factor;
		float timeDiff;

		if(k2.time < k1.time){
			//factor = 
			timeDiff = mDuration-k1.time /* time until the end of the animation */+
				+ k2.time /* time until the beggining of the first (not always first?) frame */;
			if(time >= k1.time && time <= mDuration){
				factor = (time-k1.time)/timeDiff;
			}
			else{
				factor = (timeDiff-time)/timeDiff;
			}

		}
		else{
			timeDiff = k2.time-k1.time;
			factor = (time-k1.time)/timeDiff;
		}


		if(timeDiff==0.0f){
			rotation = k1.rotation;
		}
		else{
			/* slerp */
			rotation = glm::mix(k1.rotation,
				glm::dot(k1.rotation, k2.rotation)<0?-k2.rotation:k2.rotation, factor);
		}

	}
	else if(mRotKeys.size()==1){
		rotation = mRotKeys[0].rotation;
	}

	/* Scale */

	int scaleKey = findScaleKey(time);
	if(scaleKey != -1){
		const ScaleKey& k1 = mScaleKeys[scaleKey];

		const ScaleKey& k2 = mScaleKeys[ (scaleKey+1)%mScaleKeys.size() ];

		
		float factor;
		float timeDiff;

		if(k2.time < k1.time){
			//factor = 
			timeDiff = mDuration-k1.time /* time until the end of the animation */+
				+ k2.time /* time until the beggining of the first (not always first?) frame */;
			if(time >= k1.time && time <= mDuration){
				factor = (time-k1.time)/timeDiff;
			}
			else{
				factor = (timeDiff-time)/timeDiff;
			}

		}
		else{
			timeDiff = k2.time-k1.time;
			factor = (time-k1.time)/timeDiff;
		}


		if(timeDiff==0.0f){
			scale = k1.scale;
		}
		else{
			scale = glm::mix(k1.scale, k2.scale, factor);
		}
	}
	else if(mRotKeys.size()==1){
		rotation = mRotKeys[0].rotation;
	}
}

void NodeAnimation::evaluate(float time, glm::mat4x4& dst) const{
	glm::vec3 translation, scale;
	glm::quat rotation;

	evaluate(time, translation, rotation, scale);

	// TODO remove this after converting all current animations
	scale = glm::vec3(1, 1, 1);

	dst = glm::translate(translation) * ( glm::mat4_cast(rotation) * glm::scale(scale) );
}


}; // </wt>
