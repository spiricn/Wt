#include "wt/stdafx.h"
#include "wt/ActorMoveController.h"

#include "wt/Physics.h"

#define TD_TRACE_TAG "ActorMoveController"

namespace wt
{

ActorMoveController::ActorMoveController(ASceneActor* target) : mSpeedForward(40.0f), mSpeedBackward(10.0f), mSpeedRotation(180.0f),
	mSpeedStrife(20.0f), mActor(target), mGravity(-30.0f), mMoving(false), mRotating(false), mStartedMoving(false), mStartedRotating(false),
	mStoppedMoving(false), mStoppedRotating(false), mJumpMeter(0.0f), mJumpPending(false), mPrevCollision(0), mJumpState(eJUMP_NONE), mJumpHeight(10.0f), mJumpSpeed(30){
}

void ActorMoveController::update(float dt, bool moveForward, bool moveBackward, bool strifeLeft, bool strifeRight, bool rotateLeft, bool rotateRight){
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

	if(mJumpState == eJUMP_LANDED){
		// Landed in a previous call, jump ended
		mJumpState = eJUMP_NONE;
	}
	else if(mJumpState == eJUMP_START){
		// We've jumped in the previous call, we're now rising
		mJumpState = eJUMP_RISING;
	}

	// Jump request issued
	if(mJumpPending){
		// Started jumping
		mJumpPending = false;
		mJumpState = eJUMP_START;
		mJumpMeter = mJumpHeight;
	}

	if(mJumpMeter <= 0.0f && mJumpState != eJUMP_NONE){
		// We've reached the peak and are now falling
		mJumpState = eJUMP_FALLING;
	}

	if(mJumpState == eJUMP_RISING || mJumpState == eJUMP_START){
		disp.y = mJumpSpeed;
		mJumpMeter -= disp.y * dt;
	}
	else{
		// Just apply gravity if we're not jumping
		disp.y = mGravity;
	}

	physx::PxU32 collision = mActor->getPhysicsActor()->move(disp*dt, dt, 0.000001);

	if( (collision & physx::PxControllerFlag::eCOLLISION_DOWN) &&  ! ( mPrevCollision & physx::PxControllerFlag::eCOLLISION_DOWN )){
		if(mJumpState == eJUMP_FALLING){
			mJumpState = eJUMP_LANDED;
		}
	}

	mPrevCollision = collision;
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

bool ActorMoveController::moving() const{
	return mMoving;
}

bool ActorMoveController::rotating() const{
	return mRotating;
}

void ActorMoveController::jump(){
	if(mJumpState == eJUMP_NONE){
		mJumpPending = true;
	}
}

bool ActorMoveController::startedJumping() const{
	return mJumpState == eJUMP_START;
}

bool ActorMoveController::landed() const{
	return mJumpState == eJUMP_LANDED;
}

bool ActorMoveController::jumping() const{
	return mJumpState != eJUMP_NONE;
}

void ActorMoveController::setForwardSpeed(float speed){
	mSpeedForward = speed;
}

} // </wt>

