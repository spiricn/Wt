#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/Interpolator.h>

using namespace wt;

#define TD_TRACE_TAG "ParticleDemo"


class ParticleDemo : public ADemo{
public:
	ParticleDemo() : mJetMover(0, 2*math::PI, 5, true){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void printHelp(){
		LOGI(
			"Press R to toggle rain");

		LOGI(
			"Press H for help");
	}

	void onUpdate(float dt){
		getCameraControl()->handle(dt, getInput());

		if(mRain->isVisible()){
			// Make the rain follow the camera
			glm::vec3 pos;
			getScene()->getCamera().getTranslation(pos);
			pos.y += 10.0f;

			glm::vec3 fw;
			getScene()->getCamera().getForwardVector(fw);
			fw = -fw;

			pos += fw*10.0f;

			mRain->getTransformable()->setTranslation(pos);
		}

		// Roate and move the jet
		{
			mJetMover.update(dt);

			mJet->getTransformable()->rotate(glm::vec3(1, 1, 1), 180*dt);

			glm::vec2 circPos;
			math::pointOnCircle(10, mJetMover.getValue(), circPos);

			glm::vec3 pos = mJetStartPos + glm::vec3(circPos.x, 0, circPos.y);
			mJet->getTransformable()->setTranslation(pos);
		}
	}

	void onKeyDown(VirtualKey code){
		if(code == KEY_r){
			// Toggle rain on/off
			mRain->setVisible( !mRain->isVisible() );
		}

		ADemo::onKeyDown(code);
	}

	void onDemoStart(const LuaObject& config){
		mRain = dynamic_cast<ParticleEffect*>( getScene()->findActorByName("rain") );
		WT_ASSERT(mRain, "Actor missing");

		mJet = dynamic_cast<ParticleEffect*>( getScene()->findActorByName("jet") );
		WT_ASSERT(mRain, "Actor missing");

		mJet->getTransformable()->getTranslation(mJetStartPos);
	}

	String getScriptPath() const{
		return "demo_workspace/ParticleDemo/main.lua";
	}

private:
	ParticleEffect* mRain;
	ParticleEffect* mJet;
	glm::vec3 mJetStartPos;
	FloatInterpolator mJetMover;
}; // </ParticleDemo>

WT_DECLARE_DEMO_IMPL(ParticleDemo);
