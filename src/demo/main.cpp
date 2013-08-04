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

using namespace wt;

#include "wt/lua/State.h"

class BaseClass{
public:
	void baseMethod(){
		TRACEI("trace");
	}

	virtual void virtualBaseMethod(){
		TRACEI("trace");
	}
};

class DerivedClass : public BaseClass,  public wt::lua::Object<DerivedClass>{
public:
	int cnt;

	DerivedClass (){
		cnt = 0;

	}
	void derivedMethod(){
		++cnt;
		LOGI("%d", cnt);
	}

	void virtualBaseMethod(){
		TRACEI("trace");
	}


	void generateMetaTable(){
		TRACEI("trace");
		expose("derivedMethod", &DerivedClass::derivedMethod);
		expose("virtualBaseMethod", &DerivedClass::virtualBaseMethod);

		exposeBase<BaseClass>("baseMethod", &BaseClass::baseMethod);


	}
};

int main(){
	wt::lua::State state;

	DerivedClass obj;

	LuaObject box1 = state.box(obj);
	LuaObject box2 = state.box(obj);

	state.getGlobals().Set("box1", box1);
	state.getGlobals().Set("box2", box2);

	state.createScript("test_script.lua");

}

#endif