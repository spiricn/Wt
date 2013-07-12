#ifndef WT_BASICDEMO_H
#define WT_BASICDEMO_H

#include "demo/ADemo.h"


#define TD_TRACE_TAG "BasicDemo"

namespace wt{

class BasicDemo : public ADemo{
private:
	float mRainbowFactor;

public:
	BasicDemo() : mRainbowFactor(0.0f){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		mRainbowFactor = fmod(mRainbowFactor+dt, 7.0f);

		getRenderer()->setClearColor(
			Color::fromHSV(mRainbowFactor/7.0f, 0.5f, 0.5f)
			);
	}

	void onKeyDown(VirtualKey code){
		LOGI("Key %d pressed", code);
		ADemo::onKeyDown(code);
	}

	void onMouseDown(float x, float y, MouseButton btn){
		LOGI("%s mouse button clicked @ (%d, %d)",
			(btn == BTN_LEFT ? "Left" : btn == BTN_RIGHT ? "Right" : "Middle"), (int)x, (int)y);
	}

	void onWindowSizeChanged(Uint32 w, Uint32 h){
		LOGI(
			"Window size changed to (%d, %d)", w, h);
	}

	void onKeyUp(VirtualKey code){
		LOGI(
			"Key %d released", code);

		ADemo::onKeyUp(code);
	}

	void onStart(const LuaObject& config){
		ADemo::getManager()->getInput()->setMouseGrabbed(false);

		LOGI(
			"Starting basic demo !");
	}

	void onStop(){
		LOGI(
			"Stopping basic demo !");
	}

}; // </BasicDemo>

WT_DECLARE_DEMO(BasicDemo)

}; // </wt>

#endif // </WT_BASICDEMO_H>
