/**
 * @file WindowManager.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_WINDOWMANAGER_H
#define WT_WINDOWMANAGER_H

#include "wt/gui/View.h"
#include "wt/gui/ProgressView.h"
#include "wt/gui/Button.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include "wt/gui/Checkbox.h"

#include "wt/EventManager.h"
#include "wt/EventEmitter.h"
#include "wt/gui/Layout.h"
#include "wt/gui/Window.h"

namespace wt
{

namespace gui
{

class WindowManager : public IEventListener{
public:
	typedef std::set<Window*> WindowSet;

public:
	WindowManager();

	~WindowManager();

	bool handleEvent(const EventPtr evt);

	void hook(EventManager* evtManager, AGameInput* input);

	Window* getActiveWindow() const;

	Window* createWindow();

	WindowSet::iterator getWindowsBeg();

	WindowSet::iterator getWindowsEnd();

private:
	typedef std::map<Window::ID, Window*> WindowMap;

private:
	Window* findWindow(Window::ID id);

private:
	WindowSet mWindowSet;
	WindowMap mWindows;
	EventEmitter mEventEmitter;
	EventManager* mEventManager;
	Window* mActiveWindow;
	AGameInput* mInput;
}; // </WindowManager>

} // </gui>

} // </wt>

#endif // </WT_WINDOWMANAGER_H>
