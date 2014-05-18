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

		glm::vec3 eyePos;
		getScene()->getCamera().getTranslation(eyePos);

		glm::vec3 cube2Pos;
		mCube2->getTransformable()->getTranslation(cube2Pos);

		// Handle Camera
		if(mUi->findView<gui::Checkbox>("cb_camera_look")->isChecked()){

			glm::vec3 fw;
			mCube2->getTransformable()->getForwardVector(fw);

			getScene()->getCamera().setTranslation( cube2Pos + fw * 50.0f );
			getScene()->getCamera().lookAt(cube2Pos);
		}

		
		// Handle Cube 1
		{
			mCube1->lookAt(cube2Pos);
		}

		// Handle Cube 3
		{
			glm::vec3 pos;
			mCube3->getTransformable()->getTranslation(pos);

			glm::vec3 currFw;
			mCube3->getTransformable()->getForwardVector(currFw);

			glm::vec3 endFw = glm::normalize( glm::vec3(eyePos.x, pos.y, eyePos.z) - pos );


			float delta = glm::length(currFw - endFw);
			if(delta > 00.1){
				mCube3->rotate(glm::vec3(0, 1, 0), dt*60);
			}
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

		{
			mCube3 = getScene()->findActorByName<ModelledActor>("cube_3");
			WT_ASSERT(mCube2, "Missing resource");
		}

		{
			getScene()->findActorByName<ModelledActor>("billboard")->setBillboard(true);
		}

		getRenderer()->setRenderAxes(true);

		setupGui();
	}

	void setupGui(){
		mUi = getScene()->getWindowManager()->getActiveWindow();


		gui::Checkbox*  v = mUi->createView<gui::Checkbox>("cb_camera_look");
		v->setSize(glm::vec2(200, 30));
		v->setText("Lock camera");
	}

	String getScriptPath() const{
		return "demo_workspace/TransformDemo/main.lua";
	}

private:
	gui::Layout* mUi;
	ModelledActor* mCube1;
	ModelledActor* mCube2;
	ModelledActor* mCube3;
	FloatInterpolator mCube2Interpolator;
}; // </FogDemo>

WT_DECLARE_DEMO_IMPL(TransformDemo);
