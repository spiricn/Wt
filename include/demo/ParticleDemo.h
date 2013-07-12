#ifndef WT_PARTICLE_H
#define WT_PARTICLE_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "ParticleDemo"

namespace wt{

class ParticleDemo : public ADemo{
private:
	ParticleEmitter mEmitter;
	glm::vec3 mCameraTarget;
	ParticleEffect* mRain;
	
public:
	void onRender(float dt){
		getRenderer()->render( *getScene() );

		// Render particles
		mEmitter.render(&getScene()->getCamera(),
			&getRenderer()->getFrustum());
	}

	void printHelp(){
		LOGI(
			"Press R to toggle rain");

		LOGI(
			"Press H for help");
	}

	void onUpdate(float dt){
		mEmitter.update(dt);

		getPhysics()->update(dt);
		getScene()->update(dt);

		getCameraControl()->handle(dt, getManager()->getInput());

		mRain->setParam("uPosition",
			getScene()->getCamera().getPosition() + glm::vec3(0, 20, 0)
			);
	}

	void onKeyDown(VirtualKey code){
		if(code == KEY_r){
			// Toggle rain
			mRain->setEnable( !mRain->isEnabled() );
		}

		ADemo::onKeyDown(code);
	}

	void onStart(const LuaObject& config){
		getRenderer()->setClearColor( Color::black() );

		// Create particle emitters

		mEmitter.create(150,
			getAssets()->getTextureManager()->getFromPath("$ROOT/particle_soft"), "shaders/fire.vp")
			// Fire position
			->setParam("uPosition", glm::vec3(200, 0, 200));

		mEmitter.create(400,
			getAssets()->getTextureManager()->getFromPath("$ROOT/particle_soft"), "shaders/force.vp")
			// Field center
			->setParam("uPosition", glm::vec3(202, 10, 269))
			->setParam("uSize", glm::vec3(1, 1, 1));

		mEmitter.create(500, 
			getAssets()->getTextureManager()->getFromPath("$ROOT/particle_soft"), "shaders/field.vp")
			// Field center
			->setParam("uPosition", glm::vec3(200, 10, 200))
			// Field volume
			->setParam("uSize", glm::vec3(500, 20, 500));

		mRain = mEmitter.create(1000,
			getAssets()->getTextureManager()->getFromPath("$ROOT/particle_soft"), "shaders/rain.vp");
		mRain->setEnable(false);

		// Setup lighting & color
		PointLight light;

		light.mActive = true;
		light.mPosition = glm::vec3(200, 3, 200);
		light.mColor =  Color(1, 127/255.0, 39/255.0);
		light.mAmbientIntesity = 1;
		light.mDiffuseItensity = 2;

		light.mAttenuation.constant = 0;
		light.mAttenuation.linear = 0;
		light.mAttenuation.exponential = 0.01;

		getScene()->addPointLight(light);

		light.mActive = true;
		light.mPosition = glm::vec3(202, 3, 269);
		light.mColor =  Color(0.1, 0.1, 1);

		getScene()->addPointLight(light);

		getScene()->getFog().color = Color::black();
	}

	String getConfigFile() const{
		return "assets/ParticleDemoConfig.lua";
	}

	String getLevelFile() const{
		return "assets/demo/ParticleDemo/level.lua";
	}


}; // </ParticleDemo>

WT_DECLARE_DEMO(ParticleDemo)

}; // </wt>

#endif // </WT_PARTICLE_H>
