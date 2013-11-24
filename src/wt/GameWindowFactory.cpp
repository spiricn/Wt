#include "wt/stdafx.h"

#define USE_SDL_GAME_WINDOW

#include "wt/GameWindowFactory.h"


#ifdef USE_SDL_GAME_WINDOW

#include "wt/SDLGameWindow.h"

#endif

namespace wt
{

AGameWindow* GameWindowFactory::create(const AGameWindow::Desc& desc){
#if defined(USE_SDL_GAME_WINDOW)
	SDLGameWindow* window = new SDLGameWindow;
	
	try{
		window->create(desc);
	}catch(...){
		TRACEE("Error creating SDL game window");
		return NULL;
	}

	return window;

#else
	#error No game window module defined
#endif
}


} // </wt>