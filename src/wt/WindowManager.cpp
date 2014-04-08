/**
 * @file WindowManager.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/gui/WindowManager.h"

#define TD_TRACE_TAG "WindowManager"

namespace wt
{

namespace gui
{

WindowManager::WindowManager() : mEventManager(NULL), mActiveWindow(NULL){
}

WindowManager::~WindowManager(){
	mEventManager->unregisterListener(this);

	for(WindowMap::iterator iter=mWindows.begin(); iter!=mWindows.end(); iter++){
		delete iter->second;
	}

	mWindows.clear();
}

WindowManager::WindowSet::iterator WindowManager::getWindowsBeg(){
	return mWindowSet.begin();
}

WindowManager::WindowSet::iterator WindowManager::getWindowsEnd(){
	return mWindowSet.end();
}

Window* WindowManager::findWindow(Window::ID id){
	WindowMap::iterator iter = mWindows.find(id);
	return iter == mWindows.end() ? NULL : iter->second;
}

Window* WindowManager::createWindow(){
	Window::ID id = 0;
	while(findWindow(id)){
		++id;
	}

	Window* window = new Window(id, mEventManager);

	if(mActiveWindow == NULL){
		mActiveWindow = window;
	}

	mWindowSet.insert(window);

	mWindows.insert(std::make_pair(id, window));

	return window;
}

void WindowManager::hook(EventManager* evtManager){
	mEventManager = evtManager;
	mEventManager->registerListener(this, MousePressEvent::TYPE);
	mEventManager->registerListener(this, WindowSizeChange::TYPE);
	mEventManager->registerListener(this, KeyPressEvent::TYPE);
	mEventManager->registerListener(this, MouseMotionEvent::TYPE);

	mEventEmitter.hook(mEventManager,
		3,
		ButtonClickedEvent::TYPE,
		SliderValueChangedEvent::TYPE,
		CheckboxClickedEvent::TYPE
	);
}

bool WindowManager::handleEvent(const EventPtr evt){
	return mActiveWindow->handleEvent(evt);
}

Window* WindowManager::getActiveWindow() const{
	return mActiveWindow;
}
	
} // </gui>

} // </wt>

