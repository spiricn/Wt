#include "demo/stdafx.h"

#include "demo/ADemo.h"
#include <wt/TransformableAnimator.h>
#include <wt/Interpolator.h>
#include <wt/gui/ProgressView.h>

using namespace wt;

#define TD_TRACE_TAG "FogDemo"

using namespace wt;

class FogDemo : public ADemo{
public:
	FogDemo() : mDensityInterpolator(0.1, 0.01, 10, true, FloatInterpolator::eTYPE_HALF_SINE), mColorInterpolator(0, 1.0f, 15, true){
	}

	void onUpdate(float dt){
		FogDesc fog = getScene()->getFogDesc();

		// Update fog density
		mDensityInterpolator.update(dt, fog.density);

		// Update fog color
		float hue = 0.0f;
		mColorInterpolator.update(dt, hue);

		fog.color = Color::fromHSV(hue, 0.5, 0.5);

		getScene()->setFogDesc(fog);

		// Update density progress view
		mDensityView->setProgress( (mDensityInterpolator.getValue() / 0.1f ) * 100.0f );
	}

	void onDemoStart(const LuaObject& config){
		// Start the camera animation
		getProcessManager()->attach(
			new TransformableAnimator(&getScene()->getCamera(), getAssets()->getAnimationManager()->getFromPath("$ROOT/fog_demo/camera"), "default", true)
		);

		// Setup GUI
		mDensityView = getScene()->getWindowManager()->getActiveWindow()->createView<gui::ProgressView>();
		mDensityView->setPosition(glm::vec2(0, 0));
		mDensityView->setSize(glm::vec2(300, 50));
		mDensityView->setProgressText("Fog density: ");
		mDensityView->setBackgroundColor( Color(1, 1, 1, 0.8) );
	}

	String getScriptPath() const{
		return "demo_workspace/FogDemo/main.lua";
	}

private:
	gui::ProgressView* mDensityView;

	/** Fog density interpolator */
	FloatInterpolator mDensityInterpolator;

	/** Fog color interpolator */
	FloatInterpolator mColorInterpolator;
}; // </FogDemo>

WT_DECLARE_DEMO_IMPL(FogDemo);
