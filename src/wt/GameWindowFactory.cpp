#include "wt/stdafx.h"

#define USE_SDL_GAME_WINDOW

#include "wt/GameWindowFactory.h"


#ifdef USE_SDL_GAME_WINDOW

#include "wt/SDLGameWindow.h"

#endif

namespace wt
{

AGameWindow* GameWindowFactory::create(const AGameWindow::VideoMode& desc){
#if defined(USE_SDL_GAME_WINDOW)
	SDLGameWindow* window = new SDLGameWindow;
	
	if(window->create(desc)){
		return window;
	}
	else{
		delete window;
		return NULL;
	}
#else
	#error No game window module defined
#endif
}


} // </wt>