#ifndef WT_FPSCAMERACONTROLER_H
#define WT_FPSCAMERACONTROLER_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/AGameInput.h"
#include "wt/CameraControler.h"

namespace wt{

namespace math{

class FPSCameraControler : public CameraControler{
private:
	Camera* mCamera;
	float mMoveSpeed;
	float mMouseSensitivity;

public:
	FPSCameraControler(Camera* camera=NULL) : mCamera(camera),
		mMoveSpeed(50.0f), mMouseSensitivity(0.1f){
	}

	void setCamera(Camera* camera){
		mCamera = camera;
	}
	

	void setMoveSpeed(float speed){
		mMoveSpeed = speed;
	}

	void handle(float dt, AGameInput* input){
		if(input->isKeyDown(KEY_w)){
			moveForward(dt);
		}
		else if(input->isKeyDown(KEY_s)){
			moveBackward(dt);
		}

		if(input->isKeyDown(KEY_a)){
			strifeLeft(dt);
		}
		else if(input->isKeyDown(KEY_d)){
			strifeRight(dt);
		}

		if(input->isKeyDown(KEY_SPACE)){
			moveUp(dt);
		}
		else if(input->isKeyDown(KEY_c)){
			moveDown(dt);
		}

		if(input->isMouseGrabbed()){
			lookHorizontal(input->getMouseDeltaX());
			lookVertical(input->getMouseDeltaY());
		}
	}

	void moveForward(float dt){
		mCamera->moveForward(-mMoveSpeed*dt);
	}

	void moveBackward(float dt){
		mCamera->moveForward(mMoveSpeed*dt);
	}

	void strifeLeft(float dt){
		mCamera->strife(-mMoveSpeed*dt);
	}

	void strifeRight(float dt){
		mCamera->strife(mMoveSpeed*dt);
	}

	void moveUp(float dt){
		mCamera->translate( glm::vec3(0.0f, 1.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void moveDown(float dt){
		mCamera->translate( glm::vec3(0.0f, -1.0f, 0.0f) * (mMoveSpeed*dt) );
	}

	void lookHorizontal(float dx){
		if(dx*mMouseSensitivity != 0.0f){
			mCamera->yaw(-dx*mMouseSensitivity);
		}
	}

	void lookVertical(float dy){
		if(dy*mMouseSensitivity != 0.0f){
			mCamera->pitch(-dy*mMouseSensitivity);
		}
	}
};

}; // </ math >

}; // </ wt >

#endif
