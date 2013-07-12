#ifndef WT_SDLGAMEWINDOW_H
#define WT_SDLGAMEWINDOW_H


#include "wt/stdafx.h"


#include "wt/AGameWindow.h"
#include "wt/AGameInput.h"
#include <td/td.h>

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

namespace wt{

class SDLGameWindow : public AGameWindow, public EventListener{
private:
	static const char* TAG;
	EventManager* mEvtManager;

	SDL_Surface* mScreen;

	bool updateVideoMode();

public:
	SDLGameWindow();

	~SDLGameWindow();

	bool create(const VideoMode& mode);

	bool destroy();

	void hook(EventManager* evtManager);

	bool handleEvent(const Sp<Event> evt);

	void setVsyncEnabled(bool state);

	void swapBuffers();

	bool isFullscreen() const;

	void setScreenSize(uint32_t width, uint32_t height);
	
	void setFullscreenEnabled(bool state);

	void setWindowTitle(const String& title);

}; // </SDLGameWindow>


}; //</wt>

#endif
