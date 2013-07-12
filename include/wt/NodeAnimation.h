#ifndef WT_NODEANIMATION_H
#define WT_NODEANIMATION_H

#include "wt/Buffer.h"

namespace wt{
	
class NodeAnimation{
public:
	struct PositionKey{
		glm::vec3 position;
		float time;

		PositionKey(float x=0.0f, float y=0.0f, float z=0.0f, float t=0.0f) : time(t){
			position.x = x;
			position.y = y;
			position.z = z;
		}
	};

	struct RotationKey{
		glm::quat rotation;
		float time;

		RotationKey(float x=0.0f, float y=0.0f, float z=0.0f, float w=0.0f, float t=0.0f) : time(t){
			rotation.x = x;
			rotation.y = y;
			rotation.z = z;
			rotation.w = w;
		}
	};

	struct ScaleKey{
		glm::vec3 scale;
		float time;

		ScaleKey(float sx=0.0f, float sy=0.0f, float sz=0.0f, float t=0.0f) : time(t){
			scale.x = sx;
			scale.y = sy;
			scale.z = sz;
		}
	};

	typedef std::vector<PositionKey> PosKeyList;
	typedef std::vector<RotationKey> RotKeyList;
	typedef std::vector<ScaleKey> ScaleKeyList;

private:
	PosKeyList mPosKeys;
	RotKeyList mRotKeys;
	ScaleKeyList mScaleKeys;
	String mNodeName;

	bool mUseSplines;

	float mDuration;

	int findPosKey(float time) const;

	int findRotKey(float time) const;

	int findScaleKey(float time) const;

	void calcTangent(Int32 k, glm::vec3& res) const;
public:

	NodeAnimation();

	void setTargetNode(const String& target);

	void setDuration(float dur);

	float getDuration() const;

	const String& getTargetNode();

	RotKeyList& getRotationKeys();

	PosKeyList& getPositionKeys();

	ScaleKeyList& getScaleKeys();

	void evaluate(float time, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale) const;

	void evaluate(float time, glm::mat4x4& dst) const;

}; // </NodeAnimation>

}; // </wt>


#endif
