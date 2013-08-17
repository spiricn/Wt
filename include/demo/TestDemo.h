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

		loader.load("scene.lua");

		
		lua::State* state = getManager()->getLuaState();
		
		LuaObject obj = state->box(*getEventManager());
		state->getGlobals().Set("EventManager", obj);

		LuaObject luaCamera = state->box(getScene()->getCamera());
		state->getGlobals().Set("Camera", luaCamera);
	

		LuaObject luaScene = state->box(*getScene());
		state->getGlobals().Set("Scene", luaScene);


		getScene()->getCamera().setPosition(glm::vec3(120.82, 45.07, 215.169));
		glm::quat r;
		r.x = 0.062972;
		r.y = 0.924936;
		r.z = -0.331037;
		r.w = 0.175955;

		getScene()->getCamera().setRotation(r);

		PointLight light;
		light.mAttenuation.constant = 0.0f;
		light.mAttenuation.linear = 0.0f;
		light.mAttenuation.exponential = 0.5f;

		light.mColor = Color::yellow();
		light.mPosition = glm::vec3(126.91, 30.18, 174.11);
		
		light.mDiffuseItensity = 15;
		getScene()->addPointLight(light);

		light.mColor = Color::cyan();
		light.mPosition = glm::vec3(86.58, 3, 68.56);
		light.mDiffuseItensity = 15;
		

		getScene()->addPointLight(light);

		/*lua::ScriptPtr script1;
		getProcManager().attach(new ScriptProcess(script1 = state->createScript("test_script.lua")));*/

		Model* model = getAssets()->getModelManager()->create("orb");

		// Create a sphere batch (used for point lighting)
		IcosphereBuilder::Vertex* vertices;
		uint32_t* indices;
		uint32_t numIndices, numVertices;

		IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 1);

		Buffer<Geometry::Vertex> modelVertices(numVertices);

		model->setSize(numVertices, numIndices);

		for(int i=0; i<numVertices; i++){
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

		for(int i=0; i<getScene()->getNumPointLights(); i++){
			PointLight light;
			getScene()->getPointLight(i, light);

			if(light.mActive){
				ModelledActor* actor = getScene()->createModelledActor();
				actor->setModel(model, "main");
				actor->getTransform().setPosition(light.mPosition);
			}
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
