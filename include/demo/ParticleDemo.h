#ifndef WT_PARTICLE_H
#define WT_PARTICLE_H

#include "demo/ADemo.h"

#include "wt/Lua.h"
#include "wt/SceneLoader.h"

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
			//mRain->setEnable( !mRain->isEnabled() );
		}

		ADemo::onKeyDown(code);
	}

	void onStart(const LuaObject& config){
		getRenderer()->setClearColor( Color::black() );


		getAssets()->load("level2.lua");

		ParticleEffectResource* effect;

		if(0){
			// Create a particle effect
			effect = getAssets()->getParticleResourceManager()->create("effect");

			ParticleLayerResource::LayerDesc desc;
			desc.texture = getAssets()->getTextureManager()->find("ember");
			effect->createLayer("layer", desc);

			getAssets()->serialize("workspace/level2.lua");
		}
		else{
			// Acquire particle effect
			effect = getAssets()->getParticleResourceManager()->find("effect");
		}


		ParticleEffect* e;

		e = getScene()->createParticleEffect();
		e->create(effect);
		e->getTransform().translate(0, 0, 0);

	/*	e = getScene()->createParticleEffect();
		e->create(effect);
		e->getTransform().translate(3, 0, 0);

		e = getScene()->createParticleEffect();
		e->create(effect);
		e->getTransform().translate(6, 0, 0);
*/
//
//		SceneLoader loader(getScene(), getAssets());
//		
//	// Create particle emitters
//
//
//		if(0){
//		//if(1){
//			loader.load("scene-particles.lua");
//		}
//		else{
//
//			//// Actor
//			//ModelledActor* actor = getScene()->createModelledActor("lich");
//			//actor->setModel(getAssets()->getModelManager()->find("lich_king"), "default");
//			//actor->getAnimationPlayer()->play("attack", true);
//			//actor->getAnimationPlayer()->setSpeed(0.3);
//			//actor->getTransform().setScale(1.5, 2, 1.5);
//	
//			// Effect
//			/*ParticleEffect* effect;
//			{
//				effect = getScene()->createParticleEffect();
//		
//				effect->getTransform().setPosition(0, 5, 0);
//
//				effect->createLayer("main", getAssets(), config.Get("cfg1"));
//
//			}*/
//
//			//effect->attach( getScene()->findActorByName("lich"), config.Get("bone").ToString() );
//
//			loader.save("scene-particles.lua");
//		}
//
////		/*ModelledActor* a = getScene()->createModelledActor();
////		a->setModel(getAssets()->getModelManager()->find("arthas"), "default");
////		a->getAnimationPlayer()->play("walk", true);*/
	}


	String getConfigFile() const{
		return "assets/ParticleDemoConfig.lua";
	}

	//String getLevelFile() const{
	//	//return "assets/demo/ParticleDemo/level.lua";
	//	//return "level1.lua";
	//}


}; // </ParticleDemo>

WT_DECLARE_DEMO(ParticleDemo)

}; // </wt>

#endif // </WT_PARTICLE_H>
