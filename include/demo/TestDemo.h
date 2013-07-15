#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"


#include "wt/ParticleEffect.h"
#include "wt/SceneLoader.h"

namespace wt{

class TestDemo : public ADemo{
public:

	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		getAssets()->load("level1.lua");


		SceneLoader loader(getScene(), getAssets());

		

		if(1){
		//if(0){
			// Actor 1

			{
				ModelledActor* actor = getScene()->createModelledActor("lich");
				actor->setModel(getAssets()->getModelManager()->find("lich_king"), "default");
				actor->getAnimationPlayer()->play("attack", true);
				actor->getAnimationPlayer()->setSpeed(0.3);
				actor->getTransform().setScale(1.5, 2, 1.5);
				getScene()->getCamera().lookAt( actor->getTransform().getPosition() );
			}

			
			{
				ParticleEffect* effect = getScene()->createParticleEffect();
				ParticleEffect::EffectDesc desc;

				desc.color = Color::cyan();
				desc.maxNumber = 550;
				desc.size = 0.3;
				desc.life = 2;
				desc.texture = getAssets()->getTextureManager()->getFromPath("$ROOT/particle/circle_soft");
				desc.velocity = 0.3;

				effect->getTransform().setPosition(0, 3, 0);

				effect->create(desc);

				effect->attach( getScene()->findActorByName("lich"), config.Get("bone").ToString() );
			}

			//// Actor 2
			//{
			//	ModelledActor* actor = getScene()->createModelledActor();
			//	actor->setModel(getAssets()->getModelManager()->find("cube"), "crate");

			//	actor->getTransform().setPosition(4, 5, 4);
			//	getScene()->getCamera().lookAt( actor->getTransform().getPosition() );

			//	PhysicsActor::Desc desc;

			//	desc.geometryType = PhysicsActor::eBOX_GEOMETRY;
			//	desc.controlMode = PhysicsActor::ePHYSICS_MODE;
			//	desc.type = PhysicsActor::eDYNAMIC_ACTOR;
			//	desc.geometryDesc.boxGeometry.hx = 1.0f;
			//	desc.geometryDesc.boxGeometry.hy = 1.0f;
			//	desc.geometryDesc.boxGeometry.hz = 1.0f;
			//	desc.pose = actor->getTransform();

			//	getPhysics()->createActor(actor, desc);
			//}

			//// Terrain
			//{
			//	TerrainDesc desc;
			//	desc.texture1 = getAssets()->getImageManager()->find("grass");
			//	desc.texture2 = getAssets()->getImageManager()->find("dirt");
			//	desc.texture3 = getAssets()->getImageManager()->find("stone");

			//	desc.textureMap = getAssets()->getTextureManager()->find("map1");

			//	desc.columnScale = 3.0f;
			//	desc.rowScale = 3.0f;
			//	desc.heightScale = .1f;

			//	desc.numColumns = 129;
			//	desc.numRows = 129;

			//	desc.heightmapPath = "workspace/assets/terrain/terrain";

			//	getScene()->createTerrain()->create(desc);
			//}

			loader.save("saved-actors.lua");
		}
		else{
			loader.load("saved-actors.lua");
		}
	}

	String getConfigFile() const{
		return "assets/TestDemoConfig.lua";
	}

	String getLevelFile() const{
		return "";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
