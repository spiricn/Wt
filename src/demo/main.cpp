#include "demo/stdafx.h"

using namespace wt;

#if 1

#include "demo/DemoManager.h"

// Demo declarations
WT_DECLARE_DEMO(TestDemo);
WT_DECLARE_DEMO(BasicDemo);
WT_DECLARE_DEMO(FogDemo);
WT_DECLARE_DEMO(PhysicsDemo);
WT_DECLARE_DEMO(GuiDemo);

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
	}

	DemoManager::getSingleton().start();

	return 0;
}

#else

int main(){
}

#endif