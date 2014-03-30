#ifndef WT_TPSCAMCONTROLER_H
#define WT_TPSCAMCONTROLER_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/AGameInput.h"
#include "wt/CameraControler.h"

namespace wt{

namespace math{

class TPSCameraControler : public CameraControler{
private:
	Camera* mCamera;
	float mMouseSensitivity;
	const glm::vec3* mTargetPos;
	float mTheta, mRho, mDistance;

	void updateCam(){
		glm::vec3 pos(0.0f, 2.1f, 0.0f);
		if(mTargetPos!=NULL){
			pos = glm::vec3(*mTargetPos);
			pos.y += 2;
		}

		mCamera->orbit(mDistance, mTheta, mRho, pos);
	}

public:
	TPSCameraControler(Camera* camera=NULL, const glm::vec3* target=NULL) : mCamera(camera),
		mMouseSensitivity(0.001f), mDistance(3.0f),
		mTheta(0.0f), mRho(PI/2.0f), mTargetPos(target){
	}

	void setCamera(math::Camera* camera){
		mCamera = camera;
	}

	void setTarget(const glm::vec3* target){
		mTargetPos = target;
		updateCam();
	}

	void zoomIn(float dt){
		if(dt==0.0f){
			return;
		}

		mDistance -= 3.0f*dt;
		if(mDistance < 0.0f){
			mDistance=0.01f;
		}

		updateCam();
	}

	void handle(float dt, AGameInput* input){
		lookHorizontal(input->getMouseDeltaX());
		lookVertical(input->getMouseDeltaY());

		if(input->isKeyDown(KEY_u)){
			zoomIn(dt);
		}
		else if(input->isKeyDown(KEY_i)){
			zoomOut(dt);
		}

		if(input->isKeyDown(KEY_a)){
			lookHorizontal(dt*-2000);
		}
		else if(input->isKeyDown(KEY_d)){
			lookHorizontal(dt*2000);
		}


		if(input->isKeyDown(KEY_c)){
			lookVertical(dt*-1000);
		}
		else if(input->isKeyDown(KEY_SPACE)){
			lookVertical(dt*1000);
		}
	}

	void zoomOut(float dt){
		if(dt==0.0f){
			return;
		}

		mDistance += 3.0f*dt;
		updateCam();
	}

	void lookHorizontal(float dx){
		if(dx==0.0f){
			return;
		}

		mTheta = fmod((mTheta + dx*mMouseSensitivity), 2*PI);
		updateCam();
	}

	void lookVertical(float dy){
		if(dy==0.0f){
			return;
		}

		mRho += -dy*mMouseSensitivity;

		if(mRho < 0.01f){
			mRho = 0.01f;
		}
		else if(mRho >= math::PI){
			mRho = math::PI-0.01f;
		}

		updateCam();
	}
};

}; // </wt>

}; // </wt>
#endif
