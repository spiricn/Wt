#ifndef WT_CAMERA_H
#define WT_CAMERA_H

#include <wt/lua/State.h>

#include "wt/Frustum.h"
#include "wt/Transform.h"

namespace wt{

namespace math{

class Camera : public lua::Object<Camera> {
public:
	Camera();

	float getYaw() const;

	void yaw(float deg);

	void pitch(float deg);

	void getRotation(glm::mat4& rot);

	void getRotation(glm::quat& res);

	void lookAt(const glm::vec3& point);

	void setRotation(const glm::quat& quat);

	void move(const glm::vec3& offset);

	void setPosition(const glm::vec3& pos);

	void orbit(float radius, float theta, float rho, const glm::vec3& pos);

	const glm::vec3& getForwardVec() const;

	const glm::vec3& getUpVec() const;

	const glm::vec3& getRightVec() const;

	const glm::vec3& getPosition() const;

	const float getPitch() const;

	void getMatrix(glm::mat4x4& dst, bool rotOnly=false);

	void getMVPMatrix(const glm::mat4x4& model, glm::mat4x4& dst, bool rotOnly=false);

	void getMVPMatrix(const Transform& transform, glm::mat4x4& dst, bool rotOnly=false);

	Frustum& getFrustum();

	// lua

	void generateMetaTable();
private:
	static const glm::vec3 FORWARD, UP, RIGHT;

	glm::vec3 mForward, mUp, mRight, mOrigin;
	glm::mat4x4	mViewMatrix;
	Frustum mFrustum;
	bool mUpdateMatrix, mMatrixRotOnly;

	void rotateWorld(float fAngle, float x, float y, float z);

	void rotateLocal(float fAngle, float x, float y, float z);

	void buildCameraMatrix(glm::mat4x4& dst);

	void buildMatrix(glm::mat4x4& dst, bool rotOnly);

	void localToWorld(const glm::vec3& localPoint, glm::vec3& dst, bool rotOnly);

}; // </Camera>

}; // </math>

}; // </wt>

#endif

