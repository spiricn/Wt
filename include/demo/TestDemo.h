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

	const PointLight* pl;
	TestDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		static bool flag = false;
		static float t = 0;
		if(!flag){
			t += dt;

			if( t > 1){
				PointLight::Desc d = pl->getDesc();

				d.color = Color::random();

				pl->setDesc(d);

				t = 0;
			}
		}
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

#if 0
		static bool flag = false;
		// Move spotlight to the camera
		SpotLight l;
		getScene()->getSpotLight(0, l);
		l.position = getScene()->getCamera().getPosition();
		l.direction = getScene()->getCamera().getForwardVec();
		if(!flag){
			getScene()->addSpotLight(l);
			flag = true;
		}
		else{
			getScene()->setSpotLight(0, l);
		}
#endif
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

		

		PointLight::Desc desc;
		desc.attenuation.constant = 0.0f;
		desc.attenuation.linear = 0.0f;
		desc.attenuation.quadratic = 0.5f;

		desc.diffuseIntensity  = 15;

		desc.color = Color::Yellow();
		desc.position = glm::vec3(126.91, 30.18, 174.11);
		pl = getScene()->createPointLight(desc);

		desc.color = Color::Cyan();
		desc.position = glm::vec3(86.58, 3, 68.56);
		getScene()->createPointLight(desc);

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

		/*for(int i=0; i<getScene()->getNumPointLights(); i++){
			PointLight light;
			getScene()->getPointLight(i, light);

			if(light.enabled){
				ModelledActor* actor = getScene()->createModelledActor();
				actor->setModel(model, "main");
				actor->getTransform().setPosition(light.position);
			}
		}*/
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
