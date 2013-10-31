#ifndef WT_TRANSFORMABLEANIMATOR_H
#define WT_TRANSFORMABLEANIMATOR_H

#include "wt/NodeAnimation.h"
#include "wt/ATransformable.h"
#include "wt/AProcess.h"

namespace wt
{

class TransformableAnimator : public AProcess{
public:
	enum State{
		eSTATE_INVALID = -1,
		eSTATE_PLAYING,
		eSTATE_PAUSED,
		eSTATE_FINISHED
	}; // </State>

	class IListener{
	public:
		virtual void onAnimationStateChanged(TransformableAnimator*, State) {};
	}; // </IListener>

	TransformableAnimator(ATransformable* target, NodeAnimation* animation, bool loop);

	void setSpeed(float speed);

	void pause();

	void play();

	void stop();

	void reset();

	void setListener(IListener* listener);

	// Process interface

	void onProcUpdate(float dt);

	void onProcStart();

	void onProcEnd();

private:
	void changeState(State state);

	TransformableAnimator(const TransformableAnimator&);

	State mState;
	ATransformable* mTarget;
	float mSpeed;
	NodeAnimation* mAnimation;
	float mCurrentTime;
	bool mLoop;
	IListener* mListener;

}; // </TransformableAnimator>

} // </wt>

#endif // </WT_TRANSFORMABLEANIMATOR_H>