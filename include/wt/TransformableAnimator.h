#ifndef WT_TRANSFORMABLEANIMATOR_H
#define WT_TRANSFORMABLEANIMATOR_H

#include "wt/Animation.h"
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
		virtual void onAnimationProgress(TransformableAnimator*, float) {};
	}; // </IListener>


	typedef uint32_t AnimationAttrib;

	static const AnimationAttrib eATTRIB_POSITION = 1 << 0;

	static const AnimationAttrib eATTRIB_ROTATION = 1 << 1;

	static const AnimationAttrib eATTRIB_SCALE = 1 << 2;

public:

	TransformableAnimator(ATransformable* target, NodeAnimation* animation, bool loop, bool selfDestruct=true);

	TransformableAnimator(ATransformable* target, Animation* animation, const String& node, bool loop, bool selfDestruct=true);

	void setSpeed(float speed);

	void setAnimationAttribs(AnimationAttrib attribs);

	State getState() const;

	void pause();

	void play();

	void stop();

	void reset();

	void setListener(IListener* listener);

	float getPosition() const;

	void setPosition(float pos);

	NodeAnimation* getNodeAnimation() const;

	void onProcUpdate(float dt);

	void onProcStart();

	void onProcEnd();

private:
	void changeState(State state);

	void animate();

	TransformableAnimator(const TransformableAnimator&);

private:
	State mState;
	ATransformable* mTarget;
	float mSpeed;
	NodeAnimation* mAnimation;
	float mCurrentTime;
	bool mLoop;
	IListener* mListener;
	bool mSelfDestruct;
	AnimationAttrib mAttribs;
}; // </TransformableAnimator>

} // </wt>

#endif // </WT_TRANSFORMABLEANIMATOR_H>