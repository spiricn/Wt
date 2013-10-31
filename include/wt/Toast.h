#ifndef WT_TOAST_H
#define WT_TOAST_H

#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"
#include "wt/gui/Window.h"

namespace wt
{

class Toast : public AProcess{
public:
	enum State{
		eSTATE_INVALID = -1,
		eSTATE_IDLE = 0,
		eSTATE_FADING_IN,
		eSTATE_DISPLAYING,
		eSTATE_FADING_OUT
	}; // </State>

public:
	Toast(gui::Window* parent, const glm::vec2& pos, const glm::vec2& size, Texture2D* texture);

	State getState() const;

	void onProcStart();

	void fadeOutNow();

	void changeState(State state);

	void onProcUpdate(float dt);

	void onProcEnd();

	// Builder methods
	Toast* setFadeInTime(float time);

	Toast* setFadeOutTime(float time);

	Toast* setDuration(float time);

	Toast* setLinger(bool state);

	Toast* setFadeInValue(float val);

	Toast* setFadeOutValue(float val);

private:
	gui::Window* mParent;
	gui::View* mView;
	Interpolator<float> mFadeInterpolator;

	State mState;
	float mDuration, mFadeInTime, mFadeOutTime, mFadeOutVal, mFadeInVal;
	bool mLinger;
	float mCurrentAlpha;

	void setAlpha(float a);

}; // </Toast>

}; // </wt>

#endif // </WT_TOAST_H>