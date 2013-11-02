#include "wt/stdafx.h"

#include "wt/NodeAnimation.h"
#include "wt/math.h"

#define TD_TRACE_TAG "NodeAnimation"

#define SPLINE_TENSION 0.8f

namespace wt
{
	
template<class T>
struct AnimationKeyPair{
	const T* key1;
	const T* key2;

	int32_t key1Idx;
	int32_t key2Idx;

	float factor;

	AnimationKeyPair() : key1(NULL), key2(NULL), factor(0.0f), key1Idx(-1), key2Idx(-1){
	}

}; // </AnimationKeyPair>

template<class T>
int findAnimationKey(const std::vector<T>& list, float time);

template<const class T>
void findAnimationKeys(const std::vector<T>& list, float time, AnimationKeyPair<T>* res);

template<class KeyType, class ValType>
void interpolate(const std::vector<KeyType>& keys, float time, ValType& result);

void NodeAnimation::calcTangent(int32_t k, glm::vec3& res) const{
	PositionKey* key;

	const PositionKey& prev = mPosKeys[ (k-1)<0?mPosKeys.size()-1:k-1 ];
	const PositionKey& next = mPosKeys[ (k+1)%mPosKeys.size() ];

	res = SPLINE_TENSION * (next.value - prev.value);
}

float NodeAnimation::getDuration() const{
	const float pos = mPosKeys.empty() ? 0.0f : mPosKeys[mPosKeys.size()-1].time;
	const float rot = mRotKeys.empty() ? 0.0f : mRotKeys[mRotKeys.size()-1].time;
	const float scl = mScaleKeys.empty() ? 0.0f : mScaleKeys[mScaleKeys.size()-1].time;

	return glm::max( glm::max(pos, rot), scl );
}

NodeAnimation::NodeAnimation() : mNodeName(""){
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
	glm::quat& rotation, glm::vec3& scale, bool useSplines) const{

	if(useSplines){
		AnimationKeyPair<PositionKey> keyPair;
		findAnimationKeys(mPosKeys, time, &keyPair);

		if(keyPair.key1 && keyPair.key2){
			glm::vec3 m1, m2;
			calcTangent(keyPair.key1Idx, m1);
			calcTangent(keyPair.key2Idx, m2);

			translation.x = math::hermite(keyPair.key1->value.x, keyPair.key2->value.x, m1.x, m2.x, keyPair.factor);
			translation.y = math::hermite(keyPair.key1->value.y, keyPair.key2->value.y, m1.y, m2.y, keyPair.factor);
			translation.z = math::hermite(keyPair.key1->value.z, keyPair.key2->value.z, m1.z, m2.z, keyPair.factor);
		}
		else{
			interpolate(mPosKeys, time, translation);
		}
	}
	else{
		interpolate(mPosKeys, time, translation);
	}

	interpolate(mRotKeys, time, rotation);
	interpolate(mScaleKeys, time, scale);
}

void NodeAnimation::evaluate(float time, glm::mat4x4& dst, bool useSplines) const{
	glm::vec3 translation, scale;
	glm::quat rotation;

	evaluate(time, translation, rotation, scale, useSplines);

	// TODO remove this after converting all current animations
	scale = glm::vec3(1, 1, 1);

	dst = glm::translate(translation) * ( glm::mat4_cast(rotation) * glm::scale(scale) );
}


template<class T>
int findAnimationKey(const std::vector<T>& list, float time){
	if(list.empty()){
		return -1;
	}

	const uint32_t numKeys = list.size();

	for(uint32_t i=0; i<numKeys; i++){
		if(i == numKeys -1 || time < list[i+1].time){
			return i;
		}
	}

	return list.size()-1;
}

template<const class T>
void findAnimationKeys(const std::vector<T>& list, float time, AnimationKeyPair<T>* res){
	int keyPos = findAnimationKey(list, time);

	if(keyPos == -1){
		res->key1 = NULL;
		res->key2 = NULL;
		res->factor = 0.0f;
	}
	else if(keyPos == list.size() - 1){
		res->key1 = &list[keyPos];
		res->key1Idx = keyPos;

		res->key2 = NULL;
		res->factor = 0.0f;
	}
	else{
		res->key1 = &list[keyPos];
		res->key1Idx = keyPos;

		res->key2 = &list[keyPos + 1];
		res->key2Idx = keyPos + 1;

		const float diff = (res->key2)->time - (res->key1)->time;

		const float pos = time - (res->key1)->time;

		res->factor = pos/diff;
	}
}

template<class KeyType, class ValType>
void interpolate(const std::vector<KeyType>& keys, float time, ValType& result){
	AnimationKeyPair<KeyType> keyPair;
	findAnimationKeys(keys, time, &keyPair);

	if(keyPair.key1 && keyPair.key2){
		result = glm::mix(keyPair.key1->value, keyPair.key2->value, keyPair.factor);
	}
	else if(keyPair.key1){
		result = keyPair.key1->value;
	}
#if 0
	else{
		// TODO assuming 'result' is already set to its respective identity value
		result = ValType(0.0f);
	}
#endif
}

}; // </wt>
