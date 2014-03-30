#include "wt/stdafx.h"

#include "wt/AEngineFramework.h"
#include "wt/ResourceSystemFactory.h"
#include "wt/FileSystemFactory.h"
#include "wt/Lua.h"

#define TD_TRACE_TAG "AEngineFramework"

namespace wt
{

using namespace lua;

AEngineFramework::AEngineFramework() : mInitialized(false), mWindow(NULL), mInput(NULL), 
	mEventManager(NULL), mLuaState(NULL), mLogFile(NULL), mProcessManager(NULL), mRunning(false),
	mAssets(NULL), mScene(NULL), mPhysics(NULL), mRenderer(NULL), mActiveSystems(0xFFFFFFFF),
	mPhysicsTimeMod(1.0f), mSceneTimeMod(1.0f), mProcTimeMod(1.0f), mTimeMod(1.0f){

	// Main scripting state
	mLuaState = new wt::lua::State;
}

void AEngineFramework::stopMainLoop(){
	WT_ASSERT(mRunning, "Main loop not running");

	mRunning = false;
}

void AEngineFramework::Desc::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	// Logging
	LuaObject loggingTable = luaState->newTable();
	loggingTable.Set("filePath", logging.filePath.c_str());
	loggingTable.Set("enabled", logging.enabled);
	dst.Set("logging", loggingTable);

	// Window
	LuaObject windowTable = luaState->newTable();
	window.serialize(luaState, windowTable);
	dst.Set("window", windowTable);

	// File System
	LuaObject fileSysTable = luaState->newTable();
	fileSystem.serialize(luaState, fileSysTable);
	dst.Set("file_system", fileSysTable);

	// Misc
	dst.Set("home_dir", homeDir.c_str());
	dst.Set("main_loop_step", mainLoopStep);

	// TODO physics debugging
}

void AEngineFramework::Desc::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	// Logging
	LuaObject loggingTable = src.Get("logging");
	if(loggingTable.IsTable()){
		luaConv(loggingTable.Get("filePath"), logging.filePath);
		luaConv(loggingTable.Get("enabled"), logging.enabled);
	}

	// Window
	LuaObject windowTable = src.Get("window");
	window.deserialize(luaState, windowTable);

	// File system
	LuaObject fileSysTable = src.Get("file_system");
	fileSystem.deserialize(luaState, fileSysTable);

	// Misc
	luaConv(src.Get("home_dir"), homeDir);
	luaConv(src.Get("main_loop_step"), mainLoopStep);
}

void AEngineFramework::processEvent(const wt::Sp<wt::Event> evt){
		const EvtType& type = evt->getType();

	if(type == KeyPressEvent::TYPE){
		const KeyPressEvent* e = static_cast<const KeyPressEvent*>(evt.get());

		if(e->mAction == KeyPressEvent::DOWN){
			onKeyDown(e->mCode);
		}
		else if(e->mAction == KeyPressEvent::UP){
			onKeyUp(e->mCode);
		}
	}
	else if(type == AppQuitEvent::TYPE){
		onAppQuit();
	}
	else if(type == WindowSizeChange::TYPE){
		const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());

		onWindowSizeChanged(e->newWidth, e->newHeight);
	}

	else if(type == MouseMotionEvent::TYPE){
		const MouseMotionEvent* e = static_cast<const MouseMotionEvent*>(evt.get());

		onMouseMotion(e);
	}

	else if(type == MousePressEvent::TYPE){
		const MousePressEvent* e = static_cast<const MousePressEvent*>(evt.get());

		if(e->mAction == MousePressEvent::eBUTTON_DOWN){
			onMouseDown(e->mX, e->mY, e->mButton);
		}
		else{
			onMouseUp(e->mX, e->mY, e->mButton);
		}
	}
}

void AEngineFramework::setTimeMod(float mod){
	mTimeMod = mod;
}

void AEngineFramework::mainLoop(){
	Timer timer;
	timer.reset();

	while(mRunning){
		float dt = timer.getSeconds() * mTimeMod;

		if(dt == 0.0f){
			dt = 1/60.0f;
		}

		// Poll inputs & queue events
		mInput->pollAndDispatch();

		// Dispatch all previously queued events
		mEventManager->tick();

		// Update all processes
		if(mActiveSystems & eSYSTEM_PROCESS){
			onBeforeSystemUpdate(eSYSTEM_PROCESS, dt * mProcTimeMod);
			mProcessManager->update(dt * mProcTimeMod);
		}
			
		if(mActiveSystems & eSYSTEM_PHYSICS){
			onBeforeSystemUpdate(eSYSTEM_PHYSICS, dt * mPhysicsTimeMod);
			mPhysics->update(dt * mPhysicsTimeMod);
		}

		if(mActiveSystems & eSYSTEM_SCENE){
			onBeforeSystemUpdate(eSYSTEM_SCENE, dt * mSceneTimeMod);
			mScene->update(dt * mSceneTimeMod);
		}

		if(mActiveSystems & eSYSTEM_SOUND){
			onBeforeSystemUpdate(eSYSTEM_SOUND, dt);
			mAssets->getSoundSystem()->update(dt);
		}
	
		// Update engine core (user defined)
		onUpdate(dt);

		if(mActiveSystems &  eSYSTEM_RENDERER){
			onBeforeSystemUpdate(eSYSTEM_RENDERER, dt);

			// Render scene
			getRenderer()->render(*mScene);
		}

		// Display the resulting frame
		mWindow->swapBuffers();
	}
}

void AEngineFramework::startMainLoop(){
	WT_ASSERT(!mRunning, "Main loop already started");

	mRunning = true;

	mainLoop();
}

void AEngineFramework::initializeFramework(const Desc& desc){
	WT_ASSERT(!mInitialized, "GameFramework already initialized");

	mDesc = desc;

	// Misc setup
	if(!mDesc.homeDir.empty()){
		utils::setHomeDir(mDesc.homeDir);
	}

	// Setup logging
	mLogFile = fopen(mDesc.logging.filePath.c_str(), "wb");

	if(mLogFile == NULL){
		LOGW("Error openning log file");
	}
	else{
		td_setFileOutput(mLogFile);
	}

	LOG("New log session %s", wt::utils::getCurrentTime("%H:%M:%S %d/%b/%Y").c_str());

	// Main event manager
	mEventManager = new EventManager(mLuaState);

	// Process manager
	mProcessManager = new ProcessManager;

	// Create game input & hook it to the event manager
	mInput = GameInputFactory::create();
	mInput->hook(mEventManager);

	// Create window & hook it to the event manager
	mWindow = GameWindowFactory::create(mDesc.window);
	mWindow->hook(mEventManager);

	// Initialize glew (must be done AFTER creating a valid OpenGL context i.e. rendering window)
	GLenum r = glewInit();
	if(r != GLEW_OK){
		WT_THROW("Error initializing glew \"%s\"",
			(const char*)glewGetErrorString(r));
	}

	// Physics engine
	mPhysics = new Physics(getEventManager());

	try{
		mPhysics->connectToVisualDebugger("127.0.0.1", 5425);
	}catch(...){
	}

	// Scene
	mScene = new Scene(mPhysics, mAssets, mEventManager, mLuaState);

	// Resource system
	mAssets = ResourceSystemFactory::create(mDesc.assets);

	mAssets->setFileSystem( FileSystemFactory::create(mDesc.fileSystem) );

	// Scene renderer
	mRenderer = new Renderer(getEventManager(), mScene);

	mRenderer->init(getWindow()->getWidth(),
		getWindow()->getHeight()
	);


	mInitialized = true;
}

AEngineFramework::~AEngineFramework(){
	// Deletion order matters !
	delete mRenderer;
	delete mScene;
	delete mPhysics;
	delete mProcessManager;
	delete mAssets;
	delete mEventManager;

	delete mWindow;
	delete mInput;

	if(mLogFile){
		fclose(mLogFile);
	}
}

Scene* AEngineFramework::getScene() const{
	return mScene;
}
	
Physics* AEngineFramework::getPhysics() const{
	return mPhysics;
}

AResourceSystem* AEngineFramework::getAssets() const{
	return mAssets;
}

Renderer* AEngineFramework::getRenderer() const{
	return mRenderer;
}

AGameWindow* AEngineFramework::getWindow() const{
	return mWindow;
}

AGameInput* AEngineFramework::getInput() const{
	return mInput;
}

EventManager* AEngineFramework::getEventManager() const{
	return mEventManager;
}

ProcessManager* AEngineFramework::getProcessManager() const{
	return mProcessManager;
}

lua::State* AEngineFramework::getLuaState() const{
	return mLuaState;
}

void AEngineFramework::onWindowSizeChanged(uint32_t w, uint32_t h){
}

void AEngineFramework::onKeyUp(wt::VirtualKey code){
}

void AEngineFramework::onKeyDown(wt::VirtualKey code){
}
	
void AEngineFramework::onMouseDown(float x, float y, wt::MouseButton btn){
}

void AEngineFramework::onMouseUp(float x, float y, wt::MouseButton btn){
}

void AEngineFramework::onMouseMotion(const wt::MouseMotionEvent* evt){
}

void AEngineFramework::onAppQuit(){
	if(mRunning){
		stopMainLoop();
	}
}

uint32_t AEngineFramework::getActiveSystems() const{
	return mActiveSystems;
}

void AEngineFramework::setActiveSystems(uint32_t systems){
	mActiveSystems = systems;
}

void AEngineFramework::setSystemTimeMod(uint32_t systems, float timeMod){
	if(systems & eSYSTEM_PHYSICS){
		mPhysicsTimeMod = timeMod;
	}

	if(systems & eSYSTEM_SCENE){
		mSceneTimeMod = timeMod;
	}

	if(systems & eSYSTEM_PROCESS){
		mProcTimeMod = timeMod;
	}


	// TODO handle other systems
}

void AEngineFramework::onBeforeSystemUpdate(int32_t system, float dt){
}

} // </wt>
