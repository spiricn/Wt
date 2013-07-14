#ifndef WT_TESTDEMO_H
#define WT_TESTDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "TestDemo"


#include "wt/ParticleEffect.h"

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


		

		if(1){
			ModelledActor* actor = getScene()->createModelledActor();
			actor->setModel(getAssets()->getModelManager()->find("lich_king"), "default");
			actor->getAnimationPlayer()->play("attack", true);
			getScene()->getCamera().lookAt( actor->getTransform().getPosition() );

			LuaPlus::LuaObject t;
			LUA_NEW_TABLE(t);

			actor->serialize(getAssets(), t);

			std::ofstream out("test.actors");

			out << "ACTORS = {\n";
			Lua::serializeTable(t, out);
			out << "\n}\n";

			out.close();
		}
		else{
			LuaStateOwner state;
			state->DoFile("test.actors");

			for(LuaTableIterator iter(state->GetGlobal("ACTORS")); iter; iter.Next()){
				LuaObject& actorDesc = iter.GetValue();

				ModelledActor* actor = getScene()->createModelledActor();
				actor->deserialize(getAssets(), actorDesc);
				//printf("asdf");
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
