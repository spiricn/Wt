#include "wt/stdafx.h"

#include "wt/AEngineFramework.h"
#include "wt/ResourceSystemFactory.h"

#define TD_TRACE_TAG "AEngineFramework"

namespace wt
{

AEngineFramework::AEngineFramework() : mInitialized(false), mWindow(NULL), mInput(NULL), 
	mEventManager(NULL), mLuaState(NULL), mLogFile(NULL), mProcessManager(NULL), mRunning(false),
	mAssets(NULL), mScene(NULL), mPhysics(NULL), mRenderer(NULL){
}

void AEngineFramework::stopMainLoop(){
	WT_ASSERT(mRunning, "Main loop not running");

	mRunning = false;
}

void AEngineFramework::processEvent(const wt::Sp<wt::Event> evt){
		const EvtType& type = evt->getType();

	if(type == KeyPressEvent::TYPE){
		const KeyPressEvent* e = static_cast<const KeyPressEvent*>(evt.get());

		if(e->mAction == KeyPressEvent::DOWN){
			onKeyDown(e->mCode);
		}
		else{
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

void AEngineFramework::mainLoop(){
	Timer timer;
	TimeAccumulator timeAccumulator(mDesc.mainLoopStep);
		
	timer.reset();

	const float dt = mDesc.mainLoopStep;

	while(mRunning){
		if(!timeAccumulator.update( timer.getSeconds() )){
			continue;
		}

		// Poll inputs & queue events
		mInput->pollAndDispatch();

		// Dispatch all previously queued events
		mEventManager->tick();

		// Update all processes
		mProcessManager->update(dt);
			
		mPhysics->update(dt);

		mScene->update(dt);

		mAssets->getSoundSystem()->update(dt);
	
		// Update engine core
		onUpdate(dt);

		// Render scene
		getRenderer()->render(*mScene);

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


	// Main scripting state
	mLuaState = new wt::lua::State;
	
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

	// Scene renderer
	mRenderer = new Renderer(getEventManager());

	mRenderer->init(getWindow()->getWidth(),
		getWindow()->getHeight()
	);


	mInitialized = true;
}

AEngineFramework::~AEngineFramework(){
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

} // </wt>
