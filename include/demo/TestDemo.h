#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"

#include "wt/ParticleEffect.h"
#include "wt/SceneLoader.h"
#include "wt/lua/LuaActor.h"
#include "wt/ScriptProcess.h"
#include "wt/lua/LuaUtils.h"
#include "wt/IcosphereBuilder.h"

#include "wt/lua/LuaModule.h"
#include "wt/lua/LuaBindings.h"

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
		glm::vec3 eyePos, fw;
		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(eyePos);

		getAssets()->getSoundSystem()->setListenerForwardVec(fw);
		getAssets()->getSoundSystem()->setListenerPosition(eyePos);

		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		if(c == VirtualKey::KEY_y){
			getScene()->clear();
		}
		else{
			ADemo::onKeyDown(c);
		}
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		Scene::GodRayParams p;
		getScene()->getGodRayParams(p);
		p.enabled = true;
		getScene()->setGodRayParams(p);
	
#if 0
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
#endif
		getRenderer()->setRenderAxes(false);

		//// Expose objects

		//getManager()->getLuaState()->expose(*getScene(), "Scene");
		//getManager()->getLuaState()->expose(*getEventManager(), "EventManager");

		//lua::LuaBindings_expose(getManager()->getLuaState()->getGlobals());

		//// Create script process
		//lua::ScriptPtr mainScript;
		//getProcManager().attach( new ScriptProcess( mainScript = getManager()->getLuaState()->createScript("workspace/assets/scripts/main.lua") ) );
	}

	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

}; // </TestDemo>

WT_DECLARE_DEMO(TestDemo)

}; // </wt>

#endif // </WT_TESTDEMO_H>
