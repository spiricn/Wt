#ifndef WT_TRANSFORM_H
#define WT_TRANSFORM_H


#include "wt/stdafx.h"

#include "wt/math.h"
#include "wt/lua/Object.h"

namespace wt{

namespace math{

class Transform : public lua::Object<Transform>{
public:
	Transform(const glm::mat4&);

	Transform();

	void setMatrix(const glm::mat4& mat);

	Transform(const Transform& other);

	const glm::quat& getQuat() const;

	void getMatrix(glm::mat4x4& dst) const;

	void translate(float dx, float dy, float dz);

	void translate(const glm::vec3& delta);

	void rotate(float x, float y, float z, float dAngle);

	void setPosition(const glm::vec3& pos);

	void setRotation(float x, float y, float z, float angle);

	void setScale(const glm::vec3& scale);

	void setScale(float sx, float sy, float sz);

	void getRotation(glm::vec4& res) const;

	void setPosition(float x, float y, float z);

	void setQuat(float x, float y, float z, float w);

	void setRotation(const glm::quat& rot);

	void moveForward(float distance);

	void strife(float distance);

	void setYaw(float deg);

	void setPitch(float deg);

	float getYaw() const;

	void getPitch() const;

	glm::vec3 getForwardVec() const;

	void getFacing(glm::vec3& forward, glm::vec3& up) const;

	glm::vec3 getRightVec() const;

	glm::vec3 getUpVec() const;

	const glm::vec3& getPosition() const;

	const glm::vec3& getScale() const;

	void setFacing(const glm::vec3& fw, const glm::vec3& up);

	/** other * this */
	void postConcat(const Transform& other);

	/** this * other */
	void preConcat(const Transform& other);

	void generateMetaTable();

private:
	glm::vec3 mPosition, mScale;
	glm::quat mRotation;
	glm::mat4x4 mMatrix;
	bool mDirty;

}; // </Transform>


Transform operator*(const Transform&, const Transform&);

Transform operator*(const Transform&, const glm::mat4&);

Transform operator*(const glm::mat4&, const Transform&);

}; // </math>

}; // </wt>

#endif
