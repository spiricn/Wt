#include "wt/stdafx.h"
#include "wt/TPSCamController.h"

#define TD_TRACE_TAG "TPSCamController"

namespace wt
{

namespace math
{

TPSCameraControler::TPSCameraControler(Camera* camera, const ATransformable* target) : mCamera(camera),
	mMouseSensitivity(0.001f), mDistance(3.0f),
	mTheta(0.0f), mRho(PI/2.0f), mTarget(target){
}

void TPSCameraControler::setCamera(math::Camera* camera){
	mCamera = camera;
}

void TPSCameraControler::setTarget(const ATransformable* target){
	mTarget = target;

	//updateCam();
}

void TPSCameraControler::zoomIn(float dt){
	if(dt==0.0f){
		return;
	}

	mDistance -= 3.0f*dt;
	if(mDistance < 0.0f){
		mDistance=0.01f;
	}

	//updateCam();
}

void TPSCameraControler::handle(float dt, AGameInput* input){
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

	updateCam();
}

void TPSCameraControler::zoomOut(float dt){
	if(dt==0.0f){
		return;
	}

	mDistance += 3.0f*dt;
	//updateCam();
}

void TPSCameraControler::lookHorizontal(float dx){
	if(dx==0.0f){
		return;
	}

	mTheta = fmod((mTheta + dx*mMouseSensitivity), 2*PI);
	//updateCam();
}

void TPSCameraControler::lookVertical(float dy){
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

	//updateCam();
}

void TPSCameraControler::updateCam(){
	glm::vec3 pos;

	if(mTarget != NULL){
		mTarget->getTranslation(pos);
		pos.y += 5;
	}

	mCamera->orbit(mDistance, mTheta, mRho, pos);
}

} // </math>

} // </wt>
