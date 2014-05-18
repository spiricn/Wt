#ifndef WT_CAMERACONTROLER_H
#define WT_CAMERACONTROLER_H

#include "wt/AGameInput.h"
#include <wt/Camera.h>

namespace wt
{

class ATransformable;
class AGameInput;

class CameraController{
public:
	enum ControlMode{
		eCTRL_MODE_FPS,
		eCTRL_MODE_TPS
	}; // </ControlMode>

public:
	CameraController(AGameInput* input=NULL, ATransformable* camera=NULL);

	void setInput(AGameInput* input);

	void handle(float dt);

	void setCamera(ATransformable* camera);

	void setMode(ControlMode mode);

	void setTarget(const ATransformable* target, const glm::vec3& offset);

	void setDistance(float distance);

	float getDistance() const;

	void zoom(float delta);

	void orbitHorizontal(float delta);

	void orbitVertical(float delta);

	void setMoveSpeed(float speed);

	void moveForward(float delta);

	void moveBackward(float delta);

	void strifeLeft(float delta);

	void strifeRight(float delta);

	void moveUp(float delta);

	void moveDown(float delta);

	void lookHorizontal(float delta);

	void lookVertical(float delta);

private:
	struct TPSParams{
		float mouseSensitivity;
		const ATransformable* target;
		float theta;
		float rho;
		float distance;
		glm::vec3 offset;

		TPSParams();
	}; // </TPSParams>

	struct FPSParams{
		float moveSpeed;
		float mouseSensitivity;

		FPSParams();
	}; // </FPSParams>

private:
	void update();

private:
	ControlMode mMode;
	ATransformable* mCamera;
	TPSParams mTps;
	FPSParams mFps;
	AGameInput* mInput;
}; // </CameraControler>

} // </wt>

#endif // </WT_CAMERACONTROLER_H>
