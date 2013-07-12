#include "demo/stdafx.h"

#include "demo/DemoManager.h"

namespace wt{

#define DEMO_CONSOLE_XYWH -800, 126, 677, 342

const char* DemoManager::TAG = "DemoManager";

AGameWindow* DemoManager::getWindow() const{
	return mWindow;
}

AGameInput* DemoManager::getInput() const{
	return mInput;
}


DemoManager::DemoManager() : mActiveDemo(NULL){
}

DemoManager::~DemoManager(){
	if(mLogFile){
		fclose(mLogFile);
		mLogFile = NULL;
	}

	delete mEventManager;
	delete mInput;
	delete mWindow;
}

void DemoManager::registerDemo(CreateDemoFunc func, const String& name){
	mDemos.insert(
		std::make_pair(name, func));
}

DemoManager::CreateDemoFunc DemoManager::getDemo(const String& name){
	DemoMap::iterator i = mDemos.find(name);

	return i==mDemos.end() ? NULL : i->second;
}

void DemoManager::stopDemo(){

	if(mActiveDemo == NULL){
		return;
	}

	LOGI(TAG, "Stopping demo \"%s\" ...", mActiveDemo->getName().c_str());

	mActiveDemo->destroyDemo();

	delete mActiveDemo;

	mActiveDemo = NULL;

	mWindow->setWindowTitle("Wt Demo");
}

void DemoManager::startDemo(const String& name){
	LOGI(TAG, "Starting demo \"%s\" ...", name.c_str());

	stopDemo();

	if(getDemo(name) == NULL){
		LOGE(TAG, "No demo named \"%s\"", name.c_str());
	}

	mActiveDemo = getDemo(name)();

	mActiveDemo->setName(name);

	if(!mActiveDemo){
		WT_EXCEPT(TAG,
			"Unexisting demo named \"%s\"", name.c_str());
	}

	mActiveDemo->createDemo(this, mWindow, mInput, mEventManager);

	mActiveDemo->startDemo();

	std::stringstream title;
	title << "Wt Demo: \"" << name << "\"";
	mWindow->setWindowTitle(title.str());

	std::ofstream out(".lastdemo");
	out << name;
	out.close();

	LOGI(TAG, "Demo running");
}

void DemoManager::initialize(){
	//Console::setPosition(DEMO_CONSOLE_XYWH);

	// Initialize networking library
	net::Socket::initializeLib();

	// Setup logging
	mLogFile = fopen(LOG_FILE_PATH, "w");
	WT_ASSERT(mLogFile != NULL, "Error openning log file");

	Log::setOutput(mLogFile);
	LOG("New log session %s", wt::Utils::getCurrentTime("%H:%M:%S %d/%b/%Y").c_str());

	LOG("Setting working directory \"%s\"", WORK_DIR);

	mEventManager = new EventManager;

	// game input
	mInput = new SDLGameInput;
	mInput->hook(mEventManager);

	// Execute init script
	LuaStateOwner initState;
	if(initState->DoFile(INIT_SCRIPT_FILE)){
		WT_EXCEPT("init", "Exception", "Error executing init script");
	}

	// Create window (get parameters from previously executed script)
	mWindow = new SDLGameWindow;
	mWindow->create(AGameWindow::VideoMode(
		initState->GetGlobal("SCREEN_WIDTH").ToInteger(),
		initState->GetGlobal("SCREEN_HEIGHT").ToInteger(),
		initState->GetGlobal("WINDOW_TITLE").ToString(),
		initState->GetGlobal("VSYNC").ToInteger()==1?true:false,
		initState->GetGlobal("FULLSCREEN").ToInteger()==1?true:false
		)
	);

	mWindow->hook(mEventManager);

	// Initialize glew (must be done AFTER creating a valid OpenGL context i.e. rendering window)
	GLenum r = glewInit();
	if(r != GLEW_OK){
		WT_QEXCEPT("Error initializing glew \"%s\"",
			(const char*)glewGetErrorString(r));
	}

	
	mEventManager->registerGlobalListener(this);

	std::ifstream inf(".lastdemo");
	if(inf.is_open()){
		inf >> mStartupDemo;
	}
	else{
		mStartupDemo = initState->GetGlobal("startupDemo").ToString();
	}
}

bool DemoManager::handleEvent(const Sp<Event> evt){
	const EvtType& type = evt->getType();

	if(type == KeyPressEvent::TYPE){
		const KeyPressEvent* e = static_cast<const KeyPressEvent*>(evt.get());

		if(e->mCode == KEY_F4 && e->mAction == KeyPressEvent::UP){
			stopDemo();
		}
		else if(mActiveDemo){
			if(e->mAction == KeyPressEvent::DOWN){
				mActiveDemo->onKeyDownPriv(e->mCode);
			}
			else{
				mActiveDemo->onKeyUp(e->mCode);
			}
		}
	}

	else if(type == AppQuitEvent::TYPE){
	}

	else if(mActiveDemo &&  type == WindowSizeChange::TYPE){
		const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());
		mActiveDemo->onWindowSizeChanged(e->newWidth, e->newHeight);
	}

	else if(mActiveDemo && type == MouseMotionEvent::TYPE){
		const MouseMotionEvent* e = static_cast<const MouseMotionEvent*>(evt.get());

		mActiveDemo->onMouseMotion(e);
	}

	else if(mActiveDemo && type == MousePressEvent::TYPE){
		const MousePressEvent* e = static_cast<const MousePressEvent*>(evt.get());

		if(e->mAction == MousePressEvent::eBUTTON_DOWN){
			mActiveDemo->onMouseDown(e->mX, e->mY, e->mButton);
		}
		else{
			mActiveDemo->onMouseUp(e->mX, e->mY, e->mButton);
		}
	}

	return true;
}


void DemoManager::start(){
	startDemo(mStartupDemo);
	mainLoop();
}

void DemoManager::mainLoop(){
	Timer time;

	time.reset();
	float dt=0.0f;

	LOGI(TAG, "Main loop running.");
	
	//GLDBG("MainLoop init");
	
	while(!mInput->isKeyDown(KEY_ESC)){
		mInput->pollAndDispatch();
		mEventManager->tick();

		dt = time.getSeconds();


		if(mActiveDemo){
			if(!mActiveDemo->isRunning()){
				stopDemo();
			}
			else{
				mActiveDemo->update(dt);
			}
		}
		else{
			int cnt=0;
			for(DemoMap::iterator i=mDemos.begin(); i!=mDemos.end(); i++){
				printf("%d.) %s\n", ++cnt, i->first.c_str());
			}

			printf("Choice (1 - %d): ", cnt);

			int choice;
			fflush(stdin);
			scanf("%d", &choice);

			cnt = 0;
			for(DemoMap::iterator i=mDemos.begin(); i!=mDemos.end(); i++){
				if(++cnt == choice){
					startDemo(i->first);

					// reset timer
					time.getSeconds();
					break;
				}
			}

			if(!mActiveDemo){
				LOGE(TAG, "Invalid choice");
			}

		}
			
		mWindow->swapBuffers();
	}
}

}; // </wt>