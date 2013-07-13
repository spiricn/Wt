#ifndef WT_PARTICLE_H
#define WT_PARTICLE_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "ParticleDemo"

namespace wt{

class ParticleDemo : public ADemo{
private:
	//ParticleEmitter mEmitter;
	glm::vec3 mCameraTarget;
	ParticleEffect* mRain;
	
public:
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
		getPhysics()->update(dt);
		getScene()->update(dt);

		getCameraControl()->handle(dt, getManager()->getInput());

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

		ParticleEffect* effect = getScene()->createParticleEffect();
		ParticleEffect::EffectDesc desc;

		desc.color = Color::red();
		desc.duration = 1;
		desc.life = 0.5;
		desc.maxNumber = 300;
		desc.size = 0.2;
		desc.texture = getAssets()->getTextureManager()->getFromPath("$ROOT/particle_soft");
		desc.velocity = 23;

		effect->getTransform().setPosition(200, 0, 269);

		effect->create(desc);
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
