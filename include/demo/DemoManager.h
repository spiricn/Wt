#ifndef WT_DEMOBACKEND_H
#define WT_DEMOBACKEND_H

#include "wt/Defines.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Utils.h"
#include "wt/Camera.h"
#include "wt/Frustum.h"
#include "wt/Timer.h"
#include "wt/Scene.h"
#include "wt/Renderer.h"
#include "wt/AssimpModelLoader.h"
#include "wt/DevilImageLoader.h"
#include "wt/FPSCameraControler.h"
#include "wt/TPSCamControler.h"
#include "wt/CameraControler.h"
#include "wt/SkyBox.h"

#include "wt/Buffer.h"
#include "wt/Animation.h"
#include "wt/Font.h"
#include "wt/ImageManager.h"
#include "wt/Physics.h"
#include "wt/TextureArray.h"
#include "wt/TerrainChunk.h"
#include "wt/ModelManager.h"
#include "wt/Model.h"
#include "wt/Assets.h"
#include "wt/SDLGameWindow.h"
#include "wt/SDLGameInput.h"
#include "wt/LuaStateManager.h"
#include "wt/Font.h"
#include "wt/net/Socket.h"
#include "wt/Lua.h"

#include "demo/ADemo.h"

#define WORK_DIR "d:/documents/prog/c++/workspace/wt/"
#define LOG_FILE_PATH "demo-log.html"
#define INIT_SCRIPT_FILE "assets/ManagerConfig.lua"

namespace wt{


#define WT_FUN_ENTER do{ LOGD("TRACE", "%s {", __FUNCTION__); }while(0)
#define WT_RETURN LOGD("TRACE", "}"); return 



class DemoManager : public EventListener, public Singleton<DemoManager>{
public:
	typedef ADemo* (*CreateDemoFunc)();
	typedef std::map<String, CreateDemoFunc> DemoMap;

private:
	DemoMap mDemos;

	AGameWindow* mWindow;
	AGameInput* mInput;
	EventManager* mEventManager;
	FILE* mLogFile;
	ADemo* mActiveDemo;
	String mStartupDemo;

public:
	DemoManager();

	~DemoManager();


	AGameWindow* getWindow() const;

	AGameInput* getInput() const;

	void registerDemo(CreateDemoFunc func, const String& name);

	CreateDemoFunc getDemo(const String& name);

	void stopDemo();

	void startDemo(ADemo* demo);

	void startDemo(const String& name);

	void initialize();

	bool handleEvent(const Sp<Event> evt);

	void start();

private:
	void mainLoop();

}; // </DemoManager>

#define WT_REGISTER_DEMO(name) do{ DemoManager::getSingleton().registerDemo(createDemoFunc_ ## name, #name); }while(0)

}; // </wt>

#endif