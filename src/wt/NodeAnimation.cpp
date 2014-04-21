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
int findAnimationKey(const std::vector<T*>& list, float time);

template<const class T>
void findAnimationKeys(const std::vector<T*>& list, float time, AnimationKeyPair<T>* res);

template<class KeyType, class ValType>
void interpolate(const std::vector<KeyType*>& keys, float time, ValType& result);

template<class T>
const T* getKeyAt(const std::vector<T*>& list, float time);

void NodeAnimation::clear(){
	for(PosKeyIter iter=mPosKeys.begin(); iter!=mPosKeys.end(); iter++){
		delete (*iter);
	}
	mPosKeys.clear();

	for(RotKeyIter iter=mRotKeys.begin(); iter!=mRotKeys.end(); iter++){
		delete (*iter);
	}
	mRotKeys.clear();

	for(ScaleKeyIter iter=mScaleKeys.begin(); iter!=mScaleKeys.end(); iter++){
		delete (*iter);
	}
	mScaleKeys.clear();
}

void NodeAnimation::clone(const NodeAnimation* src, NodeAnimation* dst){
	dst->clear();

	// Meta data
	dst->mName = src->mName;
	dst->mUserData = src->mUserData;

	// Keys
	for(PosKeyList::const_iterator iter=src->mPosKeys.begin(); iter!=src->mPosKeys.end(); iter++){
		*dst->addPositionKey() = *(*iter);
	}

	for(RotKeyList::const_iterator iter=src->mRotKeys.begin(); iter!=src->mRotKeys.end(); iter++){
		*dst->addRotationKey() = *(*iter);
	}

	for(ScaleKeyList::const_iterator iter=src->mScaleKeys.begin(); iter!=src->mScaleKeys.end(); iter++){
		*dst->addScaleKey() = *(*iter);
	}
}

NodeAnimation::~NodeAnimation(){
	clear();
}

NodeAnimation::PosKeyIter NodeAnimation::getPosKeysBegin(){
	return mPosKeys.begin();
}

NodeAnimation::PosKeyIter NodeAnimation::getPosKeysEnd(){
	return mPosKeys.end();
}

NodeAnimation::RotKeyIter NodeAnimation::getRotKeysBegin(){
	return mRotKeys.begin();
}

NodeAnimation::RotKeyIter NodeAnimation::getRotKeysEnd(){
	return mRotKeys.end();
}

NodeAnimation::ScaleKeyIter NodeAnimation::getScaleKeysBegin(){
	return mScaleKeys.begin();
}

NodeAnimation::ScaleKeyIter NodeAnimation::getScaleKeysEnd(){
	return mScaleKeys.end();
}

uint32_t NodeAnimation::getNumPosKeys() const{
	return mPosKeys.size();
}

uint32_t NodeAnimation::getNumRotKeys() const{
	return mRotKeys.size();
}

uint32_t NodeAnimation::getNumScaleKeys() const{
	return mScaleKeys.size();
}

NodeAnimation::ScaleKey* NodeAnimation::addScaleKey(){
	ScaleKey* res;
	mScaleKeys.push_back(res = new ScaleKey(mScaleKeys.size()));
	return res;
}

NodeAnimation::RotationKey* NodeAnimation::addRotationKey(){
	RotationKey* res;
	mRotKeys.push_back(res = new RotationKey(mRotKeys.size()));
	return res;
}

NodeAnimation::PositionKey* NodeAnimation::addPositionKey(){
	PositionKey* res;
	mPosKeys.push_back(res = new PositionKey(mPosKeys.size()));
	return res;
}

void NodeAnimation::deleteScaleKey(ScaleKey* key){
	ScaleKeyIter iter = std::find(mScaleKeys.begin(), mScaleKeys.end(), key);

	WT_ASSERT(iter != mScaleKeys.end(), "Invalid key");

	iter =  mScaleKeys.erase( iter );

	if(iter != mScaleKeys.end()){
		(*iter)->setIndex( (*iter)->getIndex() - 1 );
	}

	delete key;
}

void NodeAnimation::deleteRotationKey(RotationKey* key){
	RotKeyIter iter = std::find(mRotKeys.begin(), mRotKeys.end(), key);

	WT_ASSERT(iter != mRotKeys.end(), "Invalid key");

	iter =  mRotKeys.erase( iter );

	if(iter != mRotKeys.end()){
		(*iter)->setIndex( (*iter)->getIndex() - 1 );
	}

	delete key;
}

void NodeAnimation::deletePositionKey(PositionKey* key){
	PosKeyIter iter = std::find(mPosKeys.begin(), mPosKeys.end(), key);

	WT_ASSERT(iter != mPosKeys.end(), "Invalid key");

	iter =  mPosKeys.erase( iter );

	if(iter != mPosKeys.end()){
		(*iter)->setIndex( (*iter)->getIndex() - 1 );
	}

	delete key;
}

void NodeAnimation::calcTangent(int32_t k, glm::vec3& res) const{
	PositionKey* key;

	const PositionKey& prev = *mPosKeys[ (k-1)<0?mPosKeys.size()-1:k-1 ];
	const PositionKey& next = *mPosKeys[ (k+1)%mPosKeys.size() ];

	res = SPLINE_TENSION * (next.value - prev.value);
}

float NodeAnimation::getDuration() const{
	const float pos = mPosKeys.empty() ? 0.0f : mPosKeys[mPosKeys.size()-1]->time;
	const float rot = mRotKeys.empty() ? 0.0f : mRotKeys[mRotKeys.size()-1]->time;
	const float scl = mScaleKeys.empty() ? 0.0f : mScaleKeys[mScaleKeys.size()-1]->time;

	return glm::max( glm::max(pos, rot), scl );
}

NodeAnimation::NodeAnimation(const String& name) : mName(name), mUserData(NULL){
}

void NodeAnimation::setName(const String& target){
	mName = target;
}

const String& NodeAnimation::getName() const{
	return mName;
}

void NodeAnimation::setUserData(void* data){
	mUserData = data;
}

void* NodeAnimation::getUserData() const{
	return mUserData;
}

void NodeAnimation::evaluate(float time, glm::vec3& translation,
	glm::quat& rotation, glm::vec3& scale, bool useSplines) const{

#if 0
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
#else
	interpolate(mPosKeys, time, translation);
#endif

	interpolate(mRotKeys, time, rotation);

#if 1
	// TODO scale disabled for now ( remove this after converting all current animations )
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
#else
	interpolate(mScaleKeys, time, scale);
#endif
}

void NodeAnimation::evaluate(float time, glm::mat4x4& dst, bool useSplines) const{
	glm::vec3 translation, scale;
	glm::quat rotation;

	evaluate(time, translation, rotation, scale, useSplines);

	dst = glm::translate(translation) * ( glm::mat4_cast(rotation) * glm::scale(scale) );
}

NodeAnimation::PositionKey* NodeAnimation::getPosKeyAt(float time){
	return const_cast<PositionKey*>( getKeyAt(mPosKeys, time) );
}

NodeAnimation::RotationKey* NodeAnimation::getRotKeyAt(float time){
	return const_cast<RotationKey*>( getKeyAt(mRotKeys, time) );
}

NodeAnimation::ScaleKey* NodeAnimation::getScaleKeyAt(float time){
	return const_cast<ScaleKey*>( getKeyAt(mScaleKeys, time) );
}

template<class T>
const T* getKeyAt(const std::vector<T*>& list, float time){
	AnimationKeyPair<T> pair;
	findAnimationKeys(list, time, &pair);

	if(pair.key1 && pair.key2){
		return pair.factor > 0.5f ? pair.key2 : pair.key1;
	}
	else if(pair.key1){
		return pair.key1;
	}
	else{
		return NULL;
	}
}


template<class T>
int findAnimationKey(const std::vector<T*>& list, float time){
	if(list.empty()){
		return -1;
	}

	const uint32_t numKeys = list.size();

	for(uint32_t i=0; i<numKeys; i++){
		if(i == numKeys -1 || time < list[i+1]->time){
			return i;
		}
	}

	return list.size()-1;
}

template<const class T>
void findAnimationKeys(const std::vector<T*>& list, float time, AnimationKeyPair<T>* res){
	int keyPos = findAnimationKey(list, time);

	if(keyPos == -1){
		res->key1 = NULL;
		res->key2 = NULL;
		res->factor = 0.0f;
	}
	else if(keyPos == list.size() - 1){
		res->key1 = list[keyPos];
		res->key1Idx = keyPos;

		res->key2 = NULL;
		res->factor = 0.0f;
	}
	else{
		res->key1 = list[keyPos];
		res->key1Idx = keyPos;

		res->key2 = list[keyPos + 1];
		res->key2Idx = keyPos + 1;

		const float diff = (res->key2)->time - (res->key1)->time;

		const float pos = time - (res->key1)->time;

		res->factor = pos/diff;
	}
}

template<class KeyType, class ValType>
void interpolate(const std::vector<KeyType*>& keys, float time, ValType& result){
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
