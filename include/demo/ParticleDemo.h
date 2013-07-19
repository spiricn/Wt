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


		getAssets()->load("level1.lua");

		SceneLoader loader(getScene(), getAssets());
		
		// Create particle emitters

		//if(0){
		if(1){
			loader.load("scene-particles.lua");
		}
		else{
			// rain
			{
				ParticleEffect* effect = getScene()->createParticleEffect();
				ParticleLayer::EffectDesc desc;

				LuaObject cfg = config.Get("cfg1");

				Lua::luaConv(cfg.Get("vel.local"), desc.localVelocity);
				Lua::luaConv(cfg.Get("vel.rnd"), desc.randomVelocity);
				Lua::luaConv(cfg.Get("emissionVol"), desc.emissionVolume);
				Lua::luaConv(cfg.Get("life.min"), desc.minLife);
				Lua::luaConv(cfg.Get("life.max"), desc.maxLife);
				Lua::luaConv(cfg.Get("size.min"), desc.minSize);
				Lua::luaConv(cfg.Get("size.max"), desc.maxSize);
				Lua::luaConv(cfg.Get("size.grow"), desc.sizeGrow);
				Lua::luaConv(cfg.Get("emissionRate"), desc.emissionRate);
				Lua::luaConv(cfg.Get("particleNum"), desc.particleNumber);

				String texturePath;
				Lua::luaConv(cfg.Get("texture"), texturePath);
				desc.texture = getAssets()->getTextureManager()->getFromPath(texturePath);

				effect->getTransform().setPosition(200, 20, 269);

				effect->addLayer()->create(desc);


		/*		cfg = config.Get("cfg2");

				Lua::luaConv(cfg.Get("vel.local"), desc.localVelocity);
				Lua::luaConv(cfg.Get("vel.rnd"), desc.randomVelocity);
				Lua::luaConv(cfg.Get("emissionVol"), desc.emissionVolume);
				Lua::luaConv(cfg.Get("life.min"), desc.minLife);
				Lua::luaConv(cfg.Get("life.max"), desc.maxLife);
				Lua::luaConv(cfg.Get("size.min"), desc.minSize);
				Lua::luaConv(cfg.Get("size.max"), desc.maxSize);
				Lua::luaConv(cfg.Get("size.grow"), desc.sizeGrow);
				Lua::luaConv(cfg.Get("emissionRate"), desc.emissionRate);
				Lua::luaConv(cfg.Get("particleNum"), desc.particleNumber);
				effect->addLayer()->create(desc);*/
			}

			// explosion
			{
			}

			loader.save("scene-particles.lua");
		}

//		/*ModelledActor* a = getScene()->createModelledActor();
//		a->setModel(getAssets()->getModelManager()->find("arthas"), "default");
//		a->getAnimationPlayer()->play("walk", true);*/
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
