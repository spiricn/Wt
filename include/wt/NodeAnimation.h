#ifndef WT_NODEANIMATION_H
#define WT_NODEANIMATION_H

#include "wt/Buffer.h"

namespace wt{
	
class NodeAnimation{
public:
	struct PositionKey{
		glm::vec3 value;
		float time;

		PositionKey(float x=0.0f, float y=0.0f, float z=0.0f, float t=0.0f) : time(t){
			value.x = x;
			value.y = y;
			value.z = z;
		}
	}; // </PositionKey>

	struct RotationKey{
		glm::quat value;
		float time;

		RotationKey(float x=0.0f, float y=0.0f, float z=0.0f, float w=0.0f, float t=0.0f) : time(t){
			value.x = x;
			value.y = y;
			value.z = z;
			value.w = w;
		}
	}; // </RotationKey>

	struct ScaleKey{
		glm::vec3 value;
		float time;

		ScaleKey(float sx=0.0f, float sy=0.0f, float sz=0.0f, float t=0.0f) : time(t){
			value.x = sx;
			value.y = sy;
			value.z = sz;
		}
	}; // </ScaleKey>

	typedef std::vector<PositionKey> PosKeyList;

	typedef std::vector<RotationKey> RotKeyList;

	typedef std::vector<ScaleKey> ScaleKeyList;

public:

	NodeAnimation();

	void setTargetNode(const String& target);

	//void setDuration(float dur);

	float getDuration() const;

	const String& getTargetNode();

	RotKeyList& getRotationKeys();

	PosKeyList& getPositionKeys();

	ScaleKeyList& getScaleKeys();

	void evaluate(float time, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale, bool useSplines=false) const;

	void evaluate(float time, glm::mat4x4& dst, bool useSplines=false) const;


private:
	PosKeyList mPosKeys;
	RotKeyList mRotKeys;
	ScaleKeyList mScaleKeys;
	String mNodeName;

	float mDuration;

	void calcTangent(int32_t k, glm::vec3& res) const;

	NodeAnimation(const NodeAnimation&);

}; // </NodeAnimation>



}; // </wt>


#endif
