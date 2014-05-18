#include "wt/stdafx.h"

#include "wt/CameraController.h"

namespace wt
{

CameraController::CameraController(AGameInput* input, ATransformable* camera) : mMode(eCTRL_MODE_FPS), mInput(input), mCamera(camera){
}

CameraController::FPSParams::FPSParams() : mouseSensitivity(0.3f), moveSpeed(70.0f){
}

CameraController::TPSParams::TPSParams() : mouseSensitivity(0.01f), distance(5.0f), rho(math::PI/2.0f), target(NULL){
}

void CameraController::update(){
	if(mCamera == NULL){
		return;
	}

	if(mMode == eCTRL_MODE_TPS){
		glm::vec3 pos;

		if(mTps.target != NULL){
			mTps.target->getTranslation(pos);
			pos += mTps.offset;
		}

		mCamera->orbit(mTps.distance, mTps.theta, mTps.rho, pos);
	}
	else if(mMode == eCTRL_MODE_FPS){
	}
}

void CameraController::handle(float dt){
	WT_ASSERT(mCamera, "Camera transformable not set");

	WT_ASSERT(mInput, "Input not set");

	if(mMode == eCTRL_MODE_TPS){
		if(mInput->isMouseGrabbed()){
			orbitHorizontal(mInput->getMouseDeltaX() * mTps.mouseSensitivity);
			orbitVertical(mInput->getMouseDeltaY() * mTps.mouseSensitivity);
		}

		if(mInput->isKeyDown(KEY_u)){
			zoom(-3.0f * dt);
		}
		else if(mInput->isKeyDown(KEY_i)){
			zoom(3.0f * dt);
		}
	}
	else if(mMode == eCTRL_MODE_FPS){
		if(mInput->isKeyDown(KEY_w)){
			moveForward(mFps.moveSpeed * dt);
		}
		else if(mInput->isKeyDown(KEY_s)){
			moveBackward(mFps.moveSpeed * dt);
		}

		if(mInput->isKeyDown(KEY_a)){
			strifeLeft(mFps.moveSpeed * dt);
		}
		else if(mInput->isKeyDown(KEY_d)){
			strifeRight(mFps.moveSpeed * dt);
		}

		if(mInput->isKeyDown(KEY_SPACE)){
			moveUp(mFps.moveSpeed * dt);
		}
		else if(mInput->isKeyDown(KEY_c)){
			moveDown(mFps.moveSpeed * dt);
		}

		if(mInput->isMouseGrabbed()){
			lookHorizontal( mInput->getMouseDeltaX() * mFps.mouseSensitivity);
			lookVertical(mInput->getMouseDeltaY() * mFps.mouseSensitivity );
		}
	}

	update();
}

void CameraController::setCamera(ATransformable* camera){
	mCamera = camera;
}

void CameraController::setMode(ControlMode mode){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mMode = mode;
}

void CameraController::setTarget(const ATransformable* target, const glm::vec3& offset){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mTps.target = target;
	mTps.offset = offset;
}

void CameraController::setDistance(float distance){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mTps.distance = distance;
}

float CameraController::getDistance() const{
	WT_ASSERT(mCamera, "Camera transformable not set");

	return mTps.distance;
}

void CameraController::zoom(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mTps.distance += delta;
}

void CameraController::orbitHorizontal(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	if(delta == 0.0f){
		return;
	}

	mTps.theta = fmod((mTps.theta + delta), 2*math::PI);
}

void CameraController::orbitVertical(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	if(delta == 0.0f){
		return;
	}

	mTps.rho += delta;

	if(mTps.rho < 0.01f){
		mTps.rho = 0.01f;
	}
	else if(mTps.rho  >= math::PI){
		mTps.rho = math::PI-0.01f;
	}
}

void CameraController::setMoveSpeed(float speed){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mFps.moveSpeed = speed;
}

void CameraController::moveForward(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->moveForward(-delta);
}

void CameraController::moveBackward(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->moveForward(delta);
}

void CameraController::strifeLeft(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->strife(-delta);
}

void CameraController::strifeRight(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->strife(delta);
}

void CameraController::moveUp(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->translate( glm::vec3(0.0f, 1.0f, 0.0f) * delta);
}

void CameraController::moveDown(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	mCamera->translate( glm::vec3(0.0f, -1.0f, 0.0f) * delta );
}

void CameraController::lookHorizontal(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	if(delta == 0.0f){
		return;
	}

	mCamera->yaw(-delta);
}

void CameraController::lookVertical(float delta){
	WT_ASSERT(mCamera, "Camera transformable not set");

	if(delta == 0.0f){
		return;
	}

	mCamera->pitch(-delta);
}

void CameraController::setInput(AGameInput* input){
	mInput = input;
}

} // </wt>
