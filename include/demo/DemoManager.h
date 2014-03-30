#ifndef WT_DEMOBACKEND_H
#define WT_DEMOBACKEND_H

#include "wt/Defines.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Utils.h"
#include "wt/Camera.h"
#include "wt/Timer.h"
#include "wt/Scene.h"
#include "wt/Renderer.h"
#include "wt/AssimpModelLoader.h"
#include "wt/DevilImageLoader.h"
#include "wt/CameraController.h"
#include "wt/SkyBox.h"

#include "wt/Buffer.h"
#include "wt/Animation.h"
#include "wt/Font.h"
#include "wt/ImageManager.h"
#include "wt/Physics.h"
#include "wt/TextureArray.h"
#include "wt/ModelManager.h"
#include "wt/Model.h"
#include "wt/Assets.h"
#include "wt/GameWindowFactory.h"
#include "wt/GameInputFactory.h"
#include "wt/Font.h"
#include "wt/net/Socket.h"
#include "wt/Lua.h"

#include "demo/ADemo.h"

#define WORK_DIR "d:/documents/prog/c++/workspace/wt/"
#define LOG_FILE_PATH "demo-log.txt"
#define INIT_SCRIPT_FILE "assets/ManagerConfig.lua"

namespace wt
{

class DemoManager : public Singleton<DemoManager>{
public:
	typedef ADemo* (*CreateDemoFunc)();
	typedef std::map<String, CreateDemoFunc> DemoMap;

public:
	DemoManager();

	~DemoManager();

	void registerDemo(CreateDemoFunc func, const String& name);

	CreateDemoFunc getDemo(const String& name);

	void stopDemo();

	void startDemo(ADemo* demo);

	void startDemo(const String& name);

	void initialize();

	void start();

private:
	DemoMap mDemos;
	FILE* mLogFile;
	ADemo* mActiveDemo;
	String mStartupDemo;

}; // </DemoManager>

#define WT_REGISTER_DEMO(name) do{ DemoManager::getSingleton().registerDemo(createDemoFunc_ ## name, #name); }while(0)

}; // </wt>

#endif