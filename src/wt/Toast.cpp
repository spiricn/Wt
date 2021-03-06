#include "wt/stdafx.h"

#include "wt/Toast.h"
#include "wt/gui/TextView.h"

#define TD_TRACE_TAG "Toast"

namespace wt
{

void Toast::setAlpha(float a){
	mCurrentAlpha = a;
	Color clr = Color::White();
	clr.alpha = a;
	mView->setBackgroundColor(clr);
}

Toast::Toast(gui::Layout* parent, const glm::vec2& pos, const String& text) :  mParent(parent), mState(eSTATE_IDLE),mDuration(3), mFadeInTime(0.5), mFadeOutTime(2), mFadeOutVal(0.0f), mFadeInVal(.8f), mLinger(false){
	mView = mParent->createView<gui::TextView>();
	mView->setTextScaleMode(gui::TextView::eSCALE_AUTO);
	mView->setScalingMode(gui::View::eSCALE_MODE_FIXED);
	mView->setText(text);
	mView->setTextColor(Color::Black());
	mView->setBackgroundColor( Color(0, 0, 0, 0) );

	mView->setPosition(pos - mView->getSize()/2.0f);
}

Toast::State Toast::getState() const{
	return mState;
}

void Toast::onProcStart(){
	changeState( eSTATE_FADING_IN );

	mFadeInterpolator = Interpolator<float>(0, mFadeInVal, mFadeInTime, false,
		Interpolator<float>::eTYPE_LINEAR_FORWARD);
}

void Toast::fadeOutNow(){
	changeState( eSTATE_DISPLAYING );
	mDuration = 0.0f;
}

void Toast::changeState(State state){
	if(mState != state){
		mState = state;
	}
}

void Toast::onProcUpdate(float dt){
	if(mState == eSTATE_FADING_IN || mState == eSTATE_FADING_OUT){
		mFadeInterpolator.update(dt);

		setAlpha( mFadeInterpolator.getValue() );
			

		if(mFadeInterpolator.isFinished()){
			if(mState == eSTATE_FADING_IN){
				changeState(eSTATE_DISPLAYING);
			}
			else{
				if(!mLinger){
					killProcess();
				}
				else{
					mView->setVisible(false);
				}
			}
		}
	}
	else if(mState == eSTATE_DISPLAYING){
		mDuration -= dt;

		if(mDuration <= 0.0f){
			mFadeInterpolator = Interpolator<float>(mCurrentAlpha, mFadeOutVal, mFadeOutTime, false,
				Interpolator<float>::eTYPE_LINEAR_FORWARD);
			changeState( eSTATE_FADING_OUT );
		}
	}
}

void Toast::onProcEnd(){
	mParent->removeView(mView);
}

// Builder methods
Toast* Toast::setFadeInTime(float time){
	mFadeInTime = time;

	return this;
}

Toast* Toast::setFadeOutTime(float time){
	mFadeOutTime = time;
	
	return this;
}

Toast* Toast::setDuration(float time){
	mDuration = time;

	return this;
}

Toast* Toast::setLinger(bool state){
	mLinger = state;
	return this;
}

Toast* Toast::setFadeInValue(float val){
	mFadeInVal = val;
	return this;
}

Toast* Toast::setFadeOutValue(float val){
	mFadeOutVal = val;
	return this;
}
} // </wt>
