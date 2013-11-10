#ifndef WT_SDLGAMEWINDOW_H
#define WT_SDLGAMEWINDOW_H


#include "wt/stdafx.h"


#include "wt/AGameWindow.h"
#include "wt/AGameInput.h"
#include <td/td.h>

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

namespace wt{

class SDLGameWindow : public AGameWindow{
public:
	SDLGameWindow();

	~SDLGameWindow();

	void create(const AGameWindow::Desc& mode);

	void hook(EventManager* evtManager);

	bool handleEvent(const Sp<Event> evt);

	void setVsyncEnabled(bool state);

	void swapBuffers();

	bool isFullscreen() const;

	void setScreenSize(uint32_t width, uint32_t height);
	
	void setFullscreenEnabled(bool state);

	void setWindowTitle(const String& title);

	void setWindowMode(AGameWindow::Mode mode);

	const Desc& getDesc() const;

private:
	EventManager* mEvtManager;
	SDL_Surface* mScreen;

	void updateVideoMode();

	AGameWindow::Desc mDesc;

}; // </SDLGameWindow>


}; //</wt>

#endif
