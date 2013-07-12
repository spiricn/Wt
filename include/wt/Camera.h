#ifndef WT_CAMERA_H
#define WT_CAMERA_H


#include "wt/LuaStateManager.h"

namespace wt{

namespace math{

class Camera : public ALuaObject {
private:
	static const glm::vec3 FORWARD, UP, RIGHT;

	glm::vec3 mForward, mUp, mRight, mOrigin;
	glm::mat4x4	mViewMatrix;
	bool mUpdateMatrix, mMatrixRotOnly;

	void rotateWorld(float fAngle, float x, float y, float z);

	void rotateLocal(float fAngle, float x, float y, float z);

	void buildCameraMatrix(glm::mat4x4& dst);

	void buildMatrix(glm::mat4x4& dst, bool rotOnly);

	void localToWorld(const glm::vec3& localPoint, glm::vec3& dst, bool rotOnly);

public:
	Camera() : mForward(FORWARD), mUp(UP),
		mRight(RIGHT), mOrigin(0.0f, 0.0f, 0.0f),
		mUpdateMatrix(true), mMatrixRotOnly(false){
	}

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

	// lua
	void luaPitch(LuaObject deg);
	void luaYaw(LuaObject deg);
	void luaSetPosition(LuaObject pos);
	void luaLookAt(LuaObject pos);
	void luaOrbit(LuaObject rtr, LuaObject pos);
	void expose(LuaObject& meta);

}; // </Camera>

}; // </math>

}; // </wt>

#endif

