#ifndef WT_NODEANIMATION_H
#define WT_NODEANIMATION_H

#include "wt/Buffer.h"

namespace wt{
	
class NodeAnimation{
public:

	template<class T>
	class Keyframe{
	friend class NodeAnimation;
	public:
		T value;
		float time;

		int32_t getIndex() const{
			return mIndex;
		}

	private:
		Keyframe(int32_t index) : mIndex(index), time(0.0f){
		}

	private:
		void setIndex(int32_t index){
			mIndex = index;
		}

		int32_t mIndex;
	}; // </Keyframe>

	typedef Keyframe<glm::vec3> PositionKey;

	typedef Keyframe<glm::quat> RotationKey;

	typedef Keyframe<glm::vec3> ScaleKey;

	typedef std::vector<PositionKey*> PosKeyList;

	typedef std::vector<RotationKey*> RotKeyList;

	typedef std::vector<ScaleKey*> ScaleKeyList;

	typedef PosKeyList::iterator PosKeyIter;

	typedef ScaleKeyList::iterator ScaleKeyIter;

	typedef RotKeyList::iterator RotKeyIter;

public:
	NodeAnimation(const String& name="");

	void setName(const String& target);

	float getDuration() const;

	const String& getName() const;

	void setUserData(void* data);

	void* getUserData() const;

	void evaluate(float time, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale, bool useSplines=false) const;

	void evaluate(float time, glm::mat4x4& dst, bool useSplines=false) const;

	PositionKey* getPosKeyAt(float time);

	RotationKey* getRotKeyAt(float time);

	ScaleKey* getScaleKeyAt(float time);

	void clear();

	PosKeyIter getPosKeysBegin();

	PosKeyIter getPosKeysEnd();

	RotKeyIter getRotKeysBegin();

	RotKeyIter getRotKeysEnd();

	ScaleKeyIter getScaleKeysBegin();

	ScaleKeyIter getScaleKeysEnd();

	uint32_t getNumPosKeys() const;

	uint32_t getNumRotKeys() const;

	uint32_t getNumScaleKeys() const;

	ScaleKey* addScaleKey();

	RotationKey* addRotationKey();

	PositionKey* addPositionKey();

	void deleteScaleKey(ScaleKey* key);

	void deleteRotationKey(RotationKey* key);

	void deletePositionKey(PositionKey* key);

private:
	PosKeyList mPosKeys;
	RotKeyList mRotKeys;
	ScaleKeyList mScaleKeys;
	String mName;
	void* mUserData;

	float mDuration;

	void calcTangent(int32_t k, glm::vec3& res) const;

	NodeAnimation(const NodeAnimation&);

}; // </NodeAnimation>



}; // </wt>


#endif
