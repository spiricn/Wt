#ifndef WT_TOAST_H
#define WT_TOAST_H

#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"
#include "wt/gui/Layout.h"

namespace wt
{

namespace gui
{

class TextView;

} // </gui>

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
	Toast(gui::Layout* parent, const glm::vec2& pos, const String& text);

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
	void setAlpha(float a);

private:
	gui::Layout* mParent;
	gui::TextView* mView;
	Interpolator<float> mFadeInterpolator;
	State mState;
	float mDuration;
	float mFadeInTime;
	float mFadeOutTime;
	float mFadeOutVal;
	float mFadeInVal;
	bool mLinger;
	float mCurrentAlpha;
}; // </Toast>

}; // </wt>

#endif // </WT_TOAST_H>