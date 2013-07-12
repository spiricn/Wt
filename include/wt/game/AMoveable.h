#ifndef WT_AMOVEABLE_H
#define WT_AMOVEABLE_H

namespace wt{

class AMoveable{
public:
	virtual void setRotation(const glm::quat& quat) = 0;

	virtual void move(const glm::vec3& delta, float dt=1.0f) = 0;

	virtual void setPosition(const glm::vec3& position) = 0;

	virtual const glm::vec3& getPosition() const = 0;

	virtual const glm::quat& getRotation() const = 0;

	virtual void rotate(float x, float y, float z, float angle) = 0;
};


}; // </wt>

#endif // </WT_AMOVEABLE_H>