#ifndef WT_TOAST_H
#define WT_TOAST_H

#include "wt/ProcessManager.h"
#include "wt/Interpolator.h"
#include "wt/gui/Window.h"

namespace wt{

class Toast : public AProcess{
public:
	enum State{
		eIDLE,
		eFADING_IN,
		eDISPLAYING,
		eFADING_OUT
	};

private:
	gui::Window* mParent;
	gui::View* mView;
	Interpolator<float> mFadeInterpolator;

	State mState;
	float mDuration, mFadeInTime, mFadeOutTime, mFadeOutVal, mFadeInVal;
	bool mLinger;
	float mCurrentAlpha;

	void setAlpha(float a){
		mCurrentAlpha = a;
		Color clr = Color::White();
		clr.alpha = a;
		mView->setBackgroundColor(clr);
	}

public:
	Toast(gui::Window* parent, const glm::vec2& pos, const glm::vec2& size, Texture2D* texture) : 
		mParent(parent), mState(eIDLE), mDuration(3), mFadeInTime(0.5), mFadeOutTime(2), mFadeOutVal(0.0f), mFadeInVal(.8f), mLinger(false){
	
		gui::RectView* view = mParent->createView<gui::RectView>();
		view->setScalingMode(gui::View::eFIXED);

		view->setPosition(pos);
		view->setSize(size);
		view->setBackgroundTexture(texture);
		view->setBackgroundColor( Color(0, 0, 0, 0) );

		view->setDrawBorder(false);

		mView = view;
	}

	State getState() const{
		return mState;
	}

	void onProcStart(){
		mState = eFADING_IN;

		mFadeInterpolator = Interpolator<float>(0, mFadeInVal, mFadeInTime, false,
			Interpolator<float>::eEASE_IN_QUAD);
	}

	void fadeOutNow(){
		mState = eDISPLAYING;
		mDuration = 0.0f;
	}

	void onProcUpdate(float dt){
		if(mState == eFADING_IN || mState == eFADING_OUT){
			mFadeInterpolator.update(dt);

			setAlpha( mFadeInterpolator.getValue() );
			

			if(mFadeInterpolator.isFinished()){
				if(mState == eFADING_IN){
					mState = eDISPLAYING;
				}
				else{
					if(!mLinger){
						killProcess();
					}
				}
			}
		}
		else if(mState == eDISPLAYING){
			mDuration -= dt;

			if(mDuration <= 0.0f){
				mFadeInterpolator = Interpolator<float>(mCurrentAlpha, mFadeOutVal, mFadeOutTime, false,
					Interpolator<float>::eEASE_IN_QUAD);
				mState = eFADING_OUT;
			}
		}
	}

	void onProcEnd(){
		mParent->removeView(mView);
	}

	// Builder methods
	Toast* setFadeInTime(float time){
		mFadeInTime = time;

		return this;
	}

	Toast* setFadeOutTime(float time){
		mFadeOutTime = time;
	
		return this;
	}

	Toast* setDuration(float time){
		mDuration = time;

		return this;
	}

	Toast* setLinger(bool state){
		mLinger = state;
		return this;
	}

	Toast* setFadeInValue(float val){
		mFadeInVal = val;
		return this;
	}

	Toast* setFadeOutValue(float val){
		mFadeOutVal = val;
		return this;
	}

}; // </Toast>

}; // </wt>

#endif // </WT_TOAST_H>