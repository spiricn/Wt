#include "demo/stdafx.h"

#include "demo/ADemo.h"

using namespace wt;

#define TD_TRACE_TAG "BasicDemo"

class BasicDemo : public ADemo{
public:
	BasicDemo() : mRainbowFactor(0.0f){
	}

	void onUpdate(float dt){
		mRainbowFactor = fmod(mRainbowFactor+dt, 7.0f);

		getRenderer()->setClearColor(Color::fromHSV(mRainbowFactor/7.0f, 0.5f, 0.5f));
	}

	void onKeyDown(VirtualKey code){
		LOGI("Key %d pressed", code);

		ADemo::onKeyDown(code);
	}

	void onMouseDown(float x, float y, MouseButton btn){
		LOGI("%s mouse button clicked @ (%d, %d)",
			(btn == BTN_LEFT ? "Left" : btn == BTN_RIGHT ? "Right" : "Middle"), (int)x, (int)y);
	}

	void onWindowSizeChanged(uint32_t w, uint32_t h){
		LOGI("Window size changed to (%d, %d)", w, h);
	}

	void onKeyUp(VirtualKey code){
		LOGI("Key %d released", code);

		ADemo::onKeyUp(code);
	}

	void onDemoStart(const LuaObject&){
		LOGI("Starting basic demo !");

		getInput()->setMouseGrabbed(false);
	}

	void onDemoStop(){
		LOGI("Stopping basic demo !");
	}

private:
	float mRainbowFactor;
}; // </BasicDemo>

WT_DECLARE_DEMO_IMPL(BasicDemo);
