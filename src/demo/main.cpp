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

using namespace wt;

#if defined(WT_DEMO_NO_CONSOLE) && defined(WIN32)
	#pragma comment(linker, "/SUBSYSTEM:windows")
#endif

#if 1
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


	DemoManager::getSingleton().start();

	return 0;
}
#else



int main(){
/*
	
	Path path;
	PathNode *a, *b;
	path.insertNode(a = new PathNode(glm::vec3(0, 0, 0)));

	path.insertNode(b = new PathNode(glm::vec3(10, 0, 10)));
	b->connect(a);
	a=b;

	path.insertNode(b = new PathNode(glm::vec3(20, 0, 20)));
	b->connect(a);
	a=b;


	path.insertNode(b = new PathNode(glm::vec3(30, 0, 30)));
	b->connect(a);
	a=b;

	NodeList res;
	if(path.findPath(glm::vec3(1, 0, 1), glm::vec3(29, 0, 29), res)){
		LOGI(":D", "Path found!");

		for(NodeList::iterator i=res.begin(); i!=res.end(); i++){
			LOG("Path node %f %f %f", (*i)->getPosition().x, (*i)->getPosition().y, (*i)->getPosition().z);
		}
	}
	else{
		LOGE(":/", "Path not found");
	}*/
}

#endif
