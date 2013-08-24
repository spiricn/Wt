#ifndef WT_ATRANSFORMABLE_H
#define WT_ATRANSFORMABLE_H

namespace wt
{

class ATransformable{
public:
	virtual ~ATransformable();

	virtual void setTranslation(const glm::vec3& translation) = 0;

	virtual void setRotation(const glm::quat& rotation) = 0;

	virtual void setScale(const glm::vec3& scale) = 0;

	virtual void getScale(glm::vec3& result) const = 0;

	virtual void getTranslation(glm::vec3& result) const = 0;

	virtual void getRotation(glm::quat& result) const = 0;

	virtual void translate(const glm::vec3& offset);

	virtual void rotate(const glm::vec3& axis, float angle);

	virtual void rotate(const glm::quat& offset);

	virtual void scale(const glm::vec3& offset);

	virtual void getForwardVector(glm::vec3& result);

	virtual void getRightVector(glm::vec3& result);

	virtual void getUpVector(glm::vec3& result);

	virtual void setPitch(float angle);

	virtual void setYaw(float angle);

	virtual float getPitch() const;

	virtual float getYaw() const;

	virtual void moveForward(float distance);

	virtual void moveUp(float distance);

	virtual void strife(float distance);

	virtual void getRotation(glm::vec3& axis, float& angle) const;

	virtual void getTransformMatrix(glm::mat4& res) const;

	virtual void setTransformMatrix(const glm::mat4& mat);

	virtual void setRotation(const glm::vec3& axis, float angle);

	virtual void decomposeTransform(glm::vec3& translation, glm::vec3& scale, glm::quat& rotation) const;

	virtual void getCameraMatrix(glm::vec3& dst) const;

}; // </ATransformable>

} // </wt>


#endif // </WT_ITRANSFORMABLE_H>