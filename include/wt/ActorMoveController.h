#ifndef WT_ACTORMOVECONTROLLER_H
#define WT_ACTORMOVECONTROLLER_H

#include "wt/ASceneActor.h"

namespace wt
{

// TODO Would probably be a good idea to merge this with PhysicsActor, since all it does is provide additional features

class ActorMoveController{
public:
	enum JumpState{
		eJUMP_START,
		eJUMP_RISING,
		eJUMP_FALLING,
		eJUMP_LANDED,
		eJUMP_NONE
	}; // </JumpState>

	ActorMoveController(ASceneActor* target);

	void update(bool moveForward, bool moveBackward, bool strifeLeft, bool strifeRight, bool rotateLeft, bool rotateRight, float dt);

	bool movingForward() const;

	bool movingBackward() const;

	bool changedMoveDirection() const;

	bool startedMoving() const;

	bool startedRotating() const;

	bool stoppedMoving() const;

	bool stoppedRotating() const;

	bool isMoving();

	bool isRotating();

	void jump();

	JumpState getJumpState() const;

	bool isJumping() const;

private:
	bool mJumpPending;
	bool mStoppedMoving;
	bool mStartedMoving;
	bool mStoppedRotating;
	bool mStartedRotating;
	bool mMoving;
	bool mRotating;
	bool mChangedMoveDirection;
	bool mMovingForward;
	float mSpeedForward;
	float mGravity;
	float mSpeedBackward;
	float mSpeedRotation;
	float mSpeedStrife;
	physx::PxU32 mPrevCollision;
	ASceneActor* mActor;
	float mJumpSpeed;
	float mJumpHeight;

	float mJumpMeter;
	bool mOnGround;
	JumpState mJumpState;
}; // </ActorMoveController>

} // </wt>

#endif // </WT_ACTORMOVECONTROLLER_H>
