#include "demo/stdafx.h"

#include "demo/ADemo.h"
#include <wt/TransformableAnimator.h>
#include <wt/Interpolator.h>
#include <wt/gui/ProgressView.h>
#include <wt/ModelledActor.h>

using namespace wt;

#define TD_TRACE_TAG "TransformDemo"

using namespace wt;

class TransformDemo : public ADemo{
public:
	TransformDemo(){
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt);


		glm::vec3 lookPos;
		
		if(mUi->findView<gui::Checkbox>("cb_camera_look")->isChecked()){
			getScene()->getCamera().getTranslation(lookPos);
		}
		else{
			mCube2->getTransformable()->getTranslation(lookPos);
		}

		//getScene()->getCamera().lookAt(lookPos);
		//mCube1->lookAt(lookPos);

		{
			//glm::vec3 pos;
			mCube1->getTransformable()->moveForward(5 * dt);
		}
	}

	void onDemoStart(const LuaObject& config){
		{
			// Actor
			mCube1 = getScene()->findActorByName<ModelledActor>("cube_1");
			WT_ASSERT(mCube1, "Missing resource");

			// Animation
			Animation* animation = getAssets()->getAnimationManager()->getFromPath("$ROOT/transform_demo/route_2");
			WT_ASSERT(animation, "Missing resource");

			TransformableAnimator* animator = new TransformableAnimator(
				mCube1->getTransformable(), animation, "default", true);

			//getProcessManager()->attach(animator);

			animator->setAnimationAttribs( TransformableAnimator::eATTRIB_POSITION );
		}

		{
			mCube2 = getScene()->findActorByName<ModelledActor>("cube_2");
			WT_ASSERT(mCube2, "Missing resource");

			Animation* animation = getAssets()->getAnimationManager()->getFromPath("$ROOT/transform_demo/route_1");
			WT_ASSERT(animation, "Missing resource");

			TransformableAnimator* animator = new TransformableAnimator(
				mCube2->getTransformable(), animation, "default", true);

			getProcessManager()->attach(animator);

			animator->setAnimationAttribs( TransformableAnimator::eATTRIB_POSITION | TransformableAnimator::eATTRIB_ROTATION );
		}


		setupGui();
	}

	void setupGui(){
		mUi = getScene()->getWindowManager()->getActiveWindow();


		gui::Checkbox*  v = mUi->createView<gui::Checkbox>("cb_camera_look");
		v->setSize(glm::vec2(200, 30));
		v->setText("Look at camera");
	}

	String getScriptPath() const{
		return "demo_workspace/TransformDemo/main.lua";
	}

private:
	gui::Layout* mUi;
	ModelledActor* mCube1;
	ModelledActor* mCube2;
	FloatInterpolator mCube2Interpolator;
}; // </FogDemo>

WT_DECLARE_DEMO_IMPL(TransformDemo);
