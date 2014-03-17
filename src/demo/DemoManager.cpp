#include "demo/stdafx.h"
#include "demo/DemoManager.h"

#define TD_TRACE_TAG "DemoManager"

namespace wt
{

DemoManager::DemoManager() : mActiveDemo(NULL){
}

DemoManager::~DemoManager(){
	if(mLogFile){
		fclose(mLogFile);
		mLogFile = NULL;
	}
}
void DemoManager::registerDemo(CreateDemoFunc func, const String& name){
	mDemos.insert(
		std::make_pair(name, func));
}

DemoManager::CreateDemoFunc DemoManager::getDemo(const String& name){
	DemoMap::iterator i = mDemos.find(name);

	return i==mDemos.end() ? NULL : i->second;
}

void DemoManager::start(){

	bool running = true;

	while(running){
		startDemo(mStartupDemo);

		if(mActiveDemo == NULL){
			return;
		}

		LOGI("Stopping demo \"%s\" ...", mActiveDemo->getName().c_str());

		mActiveDemo->destroyDemo();

		delete mActiveDemo;
		mActiveDemo = NULL;

		while(true){
			// Pick a new demo
			int32_t cnt = 0;
			for(DemoMap::iterator i=mDemos.begin(); i!=mDemos.end(); i++){
				printf("%d.) %s\n", ++cnt, i->first.c_str());
			}

			printf("%d.) Quit\n", ++cnt);

			printf("Choice (1 - %d): ", cnt);

			int choice;
			fflush(stdin);
			if( scanf("%d", &choice) == 0 || choice == cnt){
				running = false;
				break;
			}

			mStartupDemo = "";
			cnt = 0;
			for(DemoMap::iterator i=mDemos.begin(); i!=mDemos.end(); i++){
				if(++cnt == choice){
					mStartupDemo = i->first;
					break;
				}
			}

			if(mStartupDemo.empty()){
				LOGE("Invalid choice");
				continue;
			}
			else{
				break;
			}
		}
	}
}

void DemoManager::startDemo(const String& name){
	LOGI("Starting demo \"%s\" ...", name.c_str());

	if(getDemo(name) == NULL){
		LOGE("No demo named \"%s\"", name.c_str());
	}

	mActiveDemo = getDemo(name)();

	mActiveDemo->setName(name);

	if(!mActiveDemo){
		WT_THROW("Unexisting demo named \"%s\"", name.c_str());
	}

	mActiveDemo->createDemo(this);

	mActiveDemo->startDemo();

	std::ofstream out(".lastdemo");
	out << name;
	out.close();

	LOGI("Demo running");
}

void DemoManager::initialize(){
	//Console::setPosition(DEMO_CONSOLE_XYWH);

	// Initialize networking library
	net::Socket::initializeLib();

	// Setup logging
	mLogFile = fopen(LOG_FILE_PATH, "w");
	WT_ASSERT(mLogFile != NULL, "Error openning log file");

	td_setFileOutput(mLogFile);

	LOG("New log session %s", wt::utils::getCurrentTime("%H:%M:%S %d/%b/%Y").c_str());

	LOG("Setting working directory \"%s\"", WORK_DIR);

	std::ifstream inf(".lastdemo");
	if(inf.is_open()){
		inf >> mStartupDemo;
	}
}

}; // </wt>