#ifndef WT_GOLDDIGGERSDEMO_H
#define WT_GOLDDIGGERSDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "GoldDiggersDemo"

namespace wt{

class OnGameUpdateEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.Set("time", time);
	}

	void deserialize(LuaObject& src){
		Lua::luaConv(src, time);
	}

public:
	static const EvtType TYPE;

	float time;

	OnGameUpdateEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </OnGameUpdateEvent>

const EvtType OnGameUpdateEvent::TYPE = "OnGameUpdate";

class GoldDiggersDemo : public ADemo{
private:
	EventTable<GoldDiggersDemo> mEventTable;
	ASoundSystem* mSoundSystem;

public:

	GoldDiggersDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){ 
		getPhysics()->update(dt);
		getScene()->update(dt*1);
		getCameraControl()->handle(dt, getManager()->getInput());

		OnGameUpdateEvent* e = new OnGameUpdateEvent;
		e->time = dt;

		getEventManager()->queueEvent(e);

		mSoundSystem->setListenerForwardVec( getScene()->getCamera().getForwardVec() );
		mSoundSystem->setListenerPosition( getScene()->getCamera().getPosition() );
		mSoundSystem->update(dt);
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& cfg){
		getCameraControl()->setCamera(&getScene()->getCamera());
		mEventTable.init(this, getEventManager());
		getEventManager()->registerInternalEvent(OnGameUpdateEvent::TYPE);
	
		LuaStateManager::getSingleton().registerObject(
			getScene(), // instance
			LuaStateManager::getSingleton().exposeObject(getScene(), "SceneMetaTable"), // meta
			"Scene" // object name
		);

		LuaStateManager::getSingleton().registerObject(
			getEventManager(), // instance
			LuaStateManager::getSingleton().exposeObject(getEventManager(), "EventManagerMetaTable"), // meta
			"EventManager" // object name
		);

		LuaStateManager::getSingleton().registerObject(
			getPhysics(), // instance
			LuaStateManager::getSingleton().exposeObject(getPhysics(), "PhysicsMetaTable"), // meta
			"Physics" // object name
		);
		
		mSoundSystem = new SFSoundSystem((SFSoundManager*)getAssets()->getSoundManager());

		LuaStateManager::getSingleton().registerObject(
			mSoundSystem, // instance
			LuaStateManager::getSingleton().exposeObject(mSoundSystem, "SoundMetaTable"), // meta
			"Sound" // object name
		);


		/*try{
			LuaStateManager::getSingleton().doFile("lua/vec3.lua");
		}catch(Exception& e){
			LOGE("Demo", e.getFullDescription().c_str());
			stopDemo();
		}*/


		getUi().setGridSize(10, 20);
		getUi().setDefaultScaleMode(Gui::View::eGRID);

		
		try{
			LuaStateManager::getSingleton().doFile("assets/demo/GoldDiggersDemo/game.lua");
		}catch(Exception& e){
			LOGE("Demo", e.getFullDescription().c_str());
			system("pause");
		}

		//getRenderer()->setRenderBones(true);
	}

	void onTargetLost(){
		Gui::RectView* v = getUi().findView<Gui::RectView>("target");
		v->setBackgroundColor(Color::blue());
	}

	void onTargetAcquired(){
		Gui::RectView* v = getUi().findView<Gui::RectView>("target");
		v->setBackgroundColor(Color::red());
	}
	
	String getConfigFile() const{
		return "assets/GoldDiggersDemoConfig.lua";
	}

	String getLevelFile() const{
		return "assets/demo/GoldDiggersDemo/level.lua";
	}

}; // </GoldDiggersDemo>

WT_DECLARE_DEMO(GoldDiggersDemo)

}; // </wt>

#endif // </WT_GOLDDIGGERSDEMO_H>
