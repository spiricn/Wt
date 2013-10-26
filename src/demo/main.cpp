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
#include "wt/lua/Object.h"

wt::lua::State state;


class B{
public:
	virtual void pureVirtualFnc() = 0;

	void baseFnc(){
		TRACEI("");
	}

	virtual void virtualFnc(){
		TRACEI("");
	}

	template<class DerivedClass>
	void baseRegister(LuaObject* obj){
		obj->RegisterObjectDirect<DerivedClass>("pureVirtualFnc", (DerivedClass*)0, &DerivedClass::pureVirtualFnc);

		obj->RegisterObjectDirect("baseFnc", (B*)0, &B::baseFnc);
	}

};

class Test : public wt::lua::Object<Test>, public B{
public:
	Test(){
		
	}

	void pureVirtualFnc(){
		TRACEI("");
	}

	/*virtual void virtualFnc(){
		TRACEI("");
	}*/

	float derivedFnc(float a){
		TRACEI("");
		return a+1;
	}
	
	void generateMetaTable(){
		baseRegister<Test>(getMetaInfo()->meta);

		expose("derivedFnc", &Test::derivedFnc);

		//expose("virtualFnc", &Test::virtualFnc);

		exposeBase<B>("virtualFnc",  &B::virtualFnc);
	}
};

float Test_derivedFnc(void* test, float a){
	return static_cast<Test*>(test)->derivedFnc(a);
}

int main(){
	Test* instance = new Test;

	void* ptrInstance = instance;;

	B* objBase = reinterpret_cast<B*>(ptrInstance);

	objBase->virtualFnc();


	

	//state.getGlobals().RegisterDirect("Test_derivedFnc", Test_derivedFnc);

	//state.getGlobals().Set("obj", &obj);

	//wt::lua::ScriptPtr s = state.createScript("test.lua");

	//LuaFunction<void>( s->getState().Get("main") ) ();
}

#endif