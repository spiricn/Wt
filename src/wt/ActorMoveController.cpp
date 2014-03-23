#include "wt/stdafx.h"
#include "wt/ActorMoveController.h"

#include "wt/Physics.h"

#define TD_TRACE_TAG "ActorMoveController"

namespace wt
{

ActorMoveController::ActorMoveController(ASceneActor* target) : mSpeedForward(40.0f), mSpeedBackward(10.0f), mSpeedRotation(180.0f),
	mSpeedStrife(20.0f), mActor(target), mGravity(-30.0f), mMoving(false), mRotating(false), mStartedMoving(false), mStartedRotating(false),
	mStoppedMoving(false), mStoppedRotating(false), mJumpMeter(0.0f), mJumpPending(false){
}

void ActorMoveController::update(bool moveForward, bool moveBackward, bool strifeLeft, bool strifeRight, bool rotateLeft, bool rotateRight, float dt){
	// About to move
	const bool move = moveForward || moveBackward || strifeLeft || strifeRight;

	const bool rotate = rotateLeft || rotateRight;

	mStartedMoving = !mMoving && move;

	mStoppedMoving = mMoving && !move;

	mStartedRotating = !mRotating && rotate;

	mStoppedRotating = mRotating && !rotate;

	mMoving = move;

	mRotating = rotate;

	glm::vec3 disp(0.0f, 0.0f, 0.0f);

	if(rotate){
		mActor->getTransformable()->rotate(glm::vec3(0.0f, 1.0f, 0.0f), mSpeedRotation * ( rotateLeft ? -1.0f : 1.0f ) * dt);
	}

	// Get the actor forward vector
	glm::vec3 forward;
	mActor->getTransformable()->getForwardVector(forward);
	forward = glm::normalize( glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec3(1.0f, 0.0f, 1.0f) );

	glm::vec3 right = glm::normalize( glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)) );


	if(moveForward){
		disp += forward*mSpeedForward;
		mChangedMoveDirection = !mMovingForward;

		mMovingForward = true;
	}
	else if(moveBackward){
		disp += forward*-mSpeedBackward;

		mChangedMoveDirection = mMovingForward;

		mMovingForward = false;
	}

	if(strifeLeft){
		disp += right*mSpeedStrife;
	}

	if(strifeRight){
		disp += right*-mSpeedStrife;
	}


	// Handle jumping

	if(mJumpPending){
		// Started jumping
		mJumpPending = false;
		mJumping = true;
		mStartedJumping = true;
		mJumpMeter = 5;
	}
	else if(mJumping){
		mStartedJumping = false;
	}
	else{
		mStoppedJumping = false;
	}

	if(mJumpMeter <= 0.0f && mJumping){
		mStoppedJumping = true;
		mJumping = false;
	}

	if(mJumping){
		disp.y = 20;
		mJumpMeter -= disp.y * dt;
	}
	else{
		// Just apply gravity if we're not jumping
		disp.y = mGravity;
	}

	physx::PxVec3 pxDisp;
	pxConvert(disp, pxDisp);

	// Once we assembled the disposition vector we can move the actor
	mActor->getPhysicsActor()->getController()->move(pxDisp*dt, 0.000001, dt, physx::PxControllerFilters());
}

bool ActorMoveController::movingForward() const{
	return mMoving && mMovingForward;
}

bool ActorMoveController::movingBackward() const{
	return mMoving && !mMovingForward;
}

bool ActorMoveController::changedMoveDirection() const{
	return mMoving && mChangedMoveDirection;
}

bool ActorMoveController::startedMoving() const{
	return mStartedMoving;
}

bool ActorMoveController::startedRotating() const{
	return mStartedRotating;
}

bool ActorMoveController::stoppedMoving() const{
	return mStoppedMoving;
}

bool ActorMoveController::stoppedRotating() const{
	return mStoppedRotating;
}

bool ActorMoveController::isMoving(){
	return mMoving;
}

bool ActorMoveController::isRotating(){
	return mRotating;
}

bool ActorMoveController::isJumping() const{
	return mJumping;
}

bool ActorMoveController::startedJumping() const{
	return mStartedJumping;
}

bool ActorMoveController::stoppedJumping() const{
	return mStoppedJumping;
}

void ActorMoveController::jump(){
	if(!mJumping){
		mJumpPending = true;
	}
}

} // </wt>

