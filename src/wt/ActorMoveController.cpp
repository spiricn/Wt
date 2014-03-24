#include "wt/stdafx.h"
#include "wt/ActorMoveController.h"

#include "wt/Physics.h"

#define TD_TRACE_TAG "ActorMoveController"

namespace wt
{

ActorMoveController::ActorMoveController(ASceneActor* target) : mSpeedForward(40.0f), mSpeedBackward(10.0f), mSpeedRotation(180.0f),
	mSpeedStrife(20.0f), mActor(target), mGravity(-30.0f), mMoving(false), mRotating(false), mStartedMoving(false), mStartedRotating(false),
	mStoppedMoving(false), mStoppedRotating(false), mJumpMeter(0.0f), mJumpPending(false), mPrevCollision(0), mJumpState(eJUMP_NONE), mJumpHeight(5.0f), mJumpSpeed(12){
}


class Test : public PxQueryFilterCallback
{
public:

	/**
	\brief This filter callback is executed before the exact intersection test.

	\param[in] filterData custom filter data specified as the query's filterData.data parameter.
	\param[in] shape A shape that has not yet passed the exact intersection test.
	\param[in] actor The shape's actor.
	\param[in,out] queryFlags scene query flags from the query's function call (only flags from PxHitFlag::eMODIFIABLE_FLAGS bitmask can be modified)
	\return the updated type for this hit  (see #PxQueryHitType)
	*/
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags){
		LOG("filter data = %#x %#x %#x %#x, actor=%p",
			filterData.word0, filterData.word1, filterData.word2, filterData.word3, actor);

		return PxQueryHitType::eNONE;
	}

	/**
	\brief This filter callback is executed if the exact intersection test returned true and PxQueryFlag::ePOSTFILTER flag was set.

	\param[in] filterData custom filter data of the query
	\param[in] hit Scene query hit information. faceIndex member is not valid for overlap queries. For sweep and raycast queries the hit information can be cast to #PxSweepHit and #PxRaycastHit respectively.
	\return the updated hit type for this hit  (see #PxQueryHitType)
	*/
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit){
		return PxQueryHitType::eTOUCH;
	}
};


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

	//physx::PxVec3 pxDisp;
	//pxConvert(disp, pxDisp);

	//PxControllerFilters filters;
	//
	//static Test test;
	//filters.mFilterCallback = &test;

	////filters.mFilterData = mActor->getPhysicsActor()

	//physx::PxShape* shapes[10];
	//uint32_t numShapes = mActor->getPhysicsActor()->getController()->getActor()->getShapes(&shapes[0], 10, 0);
	//
	//physx::PxFilterData filterData = shapes[0]->getSimulationFilterData();

	//filters.mFilterData = &filterData;

	//// Once we assembled the disposition vector we can move the actor
	//physx::PxU32 collision = mActor->getPhysicsActor()->getController()->move(pxDisp*dt, 0.000001, dt, filters);

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

bool ActorMoveController::isMoving(){
	return mMoving;
}

bool ActorMoveController::isRotating(){
	return mRotating;
}

void ActorMoveController::jump(){
	if(mJumpState == eJUMP_NONE){
		mJumpPending = true;
	}
}

ActorMoveController::JumpState ActorMoveController::getJumpState() const{
	return mJumpState;
}

bool ActorMoveController::isJumping() const{
	return mJumpState != eJUMP_NONE;
}

} // </wt>

