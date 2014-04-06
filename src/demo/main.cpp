#include "demo/stdafx.h"

#if 1
using namespace wt;

#include "demo/DemoManager.h"

// Demo declarations
WT_DECLARE_DEMO(TestDemo);
WT_DECLARE_DEMO(BasicDemo);
WT_DECLARE_DEMO(FogDemo);
WT_DECLARE_DEMO(PhysicsDemo);
WT_DECLARE_DEMO(GuiDemo);
WT_DECLARE_DEMO(ParticleDemo);
WT_DECLARE_DEMO(ScriptDemo);
WT_DECLARE_DEMO(SoundDemo);

#undef WT_DEMO_NO_CONSOLE

#if defined(WT_DEMO_NO_CONSOLE) && defined(WIN32)
	#pragma comment(linker, "/SUBSYSTEM:windows")
#endif

#ifndef WT_DEMO_NO_CONSOLE
int main(){
#else
#if defined(WIN32)
	int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow){
#else
	#error Not implemented on this platform
#endif
#endif
	DemoManager::getSingleton().initialize();


	// Demo registrations
	{
		WT_REGISTER_DEMO(TestDemo);

		WT_REGISTER_DEMO(BasicDemo);

		WT_REGISTER_DEMO(FogDemo);

		WT_REGISTER_DEMO(PhysicsDemo);

		WT_REGISTER_DEMO(GuiDemo);

		WT_REGISTER_DEMO(ParticleDemo);

		WT_REGISTER_DEMO(ScriptDemo);

		WT_REGISTER_DEMO(SoundDemo);
	}

	DemoManager::getSingleton().start();

	return 0;
}

#else


#include <wt/Event.h>

class IEventListener{
public:
	virtual bool handleEvent(const wt::EvtPtr evt) = 0;
};


class AEventEmitter : public IEventEmitter{
public:
	AEventEmitter (EventManager* manager){
	}

	void emitEvent(const wt::EvtPtr evt){
	}

	bool isHooked() const{
		return false;
	}

	void hook(){
	}

	void unhook(){
	}

private:
};

//////////////////////

class DummyEvent : public wt::Event{
public:
	static wt::EvtType type;

	const wt::EvtType& getType() const{

		return type;
	}
};

wt::EvtType DummyEvent::type = "Dummy";

class DummyListener : public IEventListener{
public:
	wt::String mName;
	DummyListener(const char* name) : mName(name){
	}
	
	bool handleEvent(const wt::EvtPtr evt){
		LOG("Event %s", mName.c_str());
		return false;
	}
};


class DummyEmitter : public IEventEmitter{
public:
};

int main(){
	EventManager mngr;

	mngr.registerEvent(DummyEvent::type);

	DummyEmitter emitter;
	DummyListener l("");

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT);

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);

	mngr.unregisterListener(&l, "", &emitter);
	//mngr.registerListener(&l,
	//	DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);


	//mngr.registerListener(&l,
	//	DummyEvent::type, EventManager::eCONNECTION_DIRECT);
/*
	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);

	mngr.registerListener(&l,
		DummyEvent::type, EventManager::eCONNECTION_DIRECT, &emitter);*/


	LOG("1");
	
	mngr.emit( new DummyEvent );

	LOG("2");

	mngr.emit( new DummyEvent, &emitter);

	LOG("3");

	mngr.update();

	LOG("4");

	mngr.unregisterEvent(DummyEvent::type);
}

#endif