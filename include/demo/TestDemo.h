#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"


#include "wt/ParticleEffect.h"
#include "wt/SceneLoader.h"

#include "wt/ScriptProcess.h"
#include "wt/lua/LuaUtils.h"
#include <wt/IcosphereBuilder.h>

namespace wt{




class TestDemo : public ADemo{
public:

	ASceneActor* mMainActor;

	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getAssets()->getSoundSystem()->setListenerForwardVec( getScene()->getCamera().getForwardVec() );
		getAssets()->getSoundSystem()->setListenerPosition( getScene()->getCamera().getPosition() );

		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		const glm::vec3 start(206, 6, 205);
		const glm::vec3 end(84, 21, 205);
		const float speed = 10;

		static bool headingBack = false;

		glm::vec3 pos;
		mMainActor->getTransformable()->getTranslation(pos);

		if(headingBack){

			if( glm::length(pos-start) < 2 ){
				headingBack = false;
			}
			else{
				glm::vec3 dir = glm::normalize(start-pos);
				mMainActor->getTransformable()->translate(dir*speed*dt);
			}
		}
		else{
			if( glm::length(pos-end) < 2 ){
				headingBack = true;
			}
			else{
				glm::vec3 dir = glm::normalize(end-pos);
				mMainActor->getTransformable()->translate(dir*speed*dt);
			}
		}
	}

	void onKeyDown(VirtualKey c){

		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		getAssets()->load("level1.lua");

		SceneLoader loader(getScene(), getAssets());
		loader.load("scene.lua");

	
		{
			Model* model = getAssets()->getModelManager()->create("orb");

			// Create sphere model

			IcosphereBuilder::Vertex* vertices;
			uint32_t* indices;
			uint32_t numIndices, numVertices;

			IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 1);

			Buffer<Geometry::Vertex> modelVertices(numVertices);

			model->setSize(numVertices, numIndices);

			for(uint32_t i=0; i<numVertices; i++){
				modelVertices[i].x = vertices[i].position.x;
				modelVertices[i].y = vertices[i].position.y;
				modelVertices[i].z = vertices[i].position.z;

				modelVertices[i].nx = vertices[i].normal.x;
				modelVertices[i].nx = vertices[i].normal.y;
				modelVertices[i].nx = vertices[i].normal.z;

				modelVertices[i].s = vertices[i].texture.s;
				modelVertices[i].t = vertices[i].texture.t;
			}

			Buffer<GLuint> modelIndices(numIndices);
			memcpy(modelIndices.getData(), indices, numIndices*sizeof(uint32_t));
		
			Geometry* geometry = model->addGeometry("main", modelVertices, modelIndices);

			Model::GeometrySkin* skin = model->createSkin("main");

			skin->addMesh(geometry);

			model->create();
		}

		{
			ModelledActor* actor = getScene()->createModelledActor();
			actor->setModel(getAssets()->getModelManager()->find("orb"), "main");
			mMainActor = actor;
			actor->getTransformable()->setTranslation(glm::vec3(206, 6, 205));

			getScene()->findActorByName("lighty")->attach(mMainActor, "");
			getScene()->findActorByName("firy")->attach(mMainActor, "");
		}

		{
			Sound* sound = getScene()->createSound("");

			sound->setSound( getAssets()->getSoundSystem()->createSound("$ROOT/test") );

			sound->getSound()->setLoop(true);
			sound->getSound()->setRelativeToListener(false);
			sound->getSound()->play();


			sound->attach(mMainActor, "");
		}

		// Assemble fire
		{
			ModelledActor* model = dynamic_cast<ModelledActor*>( getScene()->findActorByName("model_fire") );
			Sound* sound = dynamic_cast<Sound*>( getScene()->findActorByName("sound_fire") );
			ParticleEffect* effect = dynamic_cast<ParticleEffect*>( getScene()->findActorByName("particle_fire") );
			PointLight* light = dynamic_cast<PointLight*>( getScene()->findActorByName("light_fire") );

			sound->attach(model, "");
			effect->attach(model, "");
			light->attach(model, "");

			mMainActor = model;
		}

		loader.save("tmp_saved_scene.lua");
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
