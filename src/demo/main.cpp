#include "demo/stdafx.h"


#include "demo/DemoManager.h"

#include "demo/GodRayDemo.h"
#include "demo/ParticleDemo.h"
#include "demo/DayNightDemo.h"
#include "demo/SoundDemo.h"
#include "demo/PhysicsDemo.h"
#include "demo/BasicDemo.h"
#include "demo/AnimationDemo.h"
#include "demo/LightDemo.h"
#include "demo/FogDemo.h"
#include "demo/OffscreenDemo.h"
#include "demo/MotionBlurDemo.h"
#include "demo/GuiDemo.h"
#include "demo/MinesDemo.h"
#include "demo/GoldDiggersDemo.h"
#include "demo/PathFindingDemo.h"
#include "demo/TestDemo.h"
#include "wt/FileIOStream.h"

using namespace wt;

#if 1

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

	WT_REGISTER_DEMO(GodRayDemo);

	WT_REGISTER_DEMO(ParticleDemo);

	WT_REGISTER_DEMO(LightDemo);

	WT_REGISTER_DEMO(DayNightDemo);

	WT_REGISTER_DEMO(PhysicsDemo);

	WT_REGISTER_DEMO(MotionBlurDemo);

	WT_REGISTER_DEMO(SoundDemo);

	WT_REGISTER_DEMO(BasicDemo);

	WT_REGISTER_DEMO(AnimationDemo);

	WT_REGISTER_DEMO(GuiDemo);

	WT_REGISTER_DEMO(FogDemo);

	WT_REGISTER_DEMO(OffscreenDemo);
	
	WT_REGISTER_DEMO(MinesDemo);

	WT_REGISTER_DEMO(GoldDiggersDemo);

	WT_REGISTER_DEMO(PathFindingDemo);

	WT_REGISTER_DEMO(TestDemo);

	DemoManager::getSingleton().start();

	return 0;
}

#else


#include "wt/lua/State.h"

int cnt = 0;
class TestClass1 : public lua::Object<TestClass1>{
public:
	
	int mcnt;
	TestClass1(){
		mcnt = ++cnt;
	}

	void test(const char* info){
		LOG("TEST CLASS 1 : id = %d (info=%s)", mcnt, info);
	}

	void generateMetaTable(LuaObject& table){
		expose("test", &TestClass1::test);
	}

};

extern "C"{
	 #include <src/lualib.h>
}
//#include "src/lua.h"
int main(){
	lua::State state;

	math::Camera cam;

	cam.setPosition(glm::vec3(3, 4, 3.15));

	state.box(cam, "Camera");


	EventManager e;
	state.box(e, "EventManager");
	state.createScript("test_script.lua");

	e.tick();

/*
	state->DoString("print( cam:getPosition())\ncam:setPosition({1, 2, 3})\nprint( cam:getPosition())\n");*/
}

#endif